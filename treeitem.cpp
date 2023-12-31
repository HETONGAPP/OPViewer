/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "treeitem.h"
#include "opcuamodel.h"
#include <QOpcUaArgument>
#include <QOpcUaAxisInformation>
#include <QOpcUaClient>
#include <QOpcUaComplexNumber>
#include <QOpcUaDoubleComplexNumber>
#include <QOpcUaEUInformation>
#include <QOpcUaExtensionObject>
#include <QOpcUaLocalizedText>
#include <QOpcUaQualifiedName>
#include <QOpcUaRange>
#include <QOpcUaXValue>
#include <QMetaEnum>
#include <QPixmap>

QT_BEGIN_NAMESPACE

const int numberOfDisplayColumns = 7; // NodeId, Value, NodeClass, DataType, BrowseName, DisplayName, Description

TreeItem::TreeItem(OpcUaModel *model) : QObject(nullptr)
    , mModel(model)
{
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, TreeItem *parent) : QObject(parent)
    , mOpcNode(node)
    , mModel(model)
    , mParentItem(parent)
{
    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::Value
                                  | QOpcUa::NodeAttribute::NodeClass
                                  | QOpcUa::NodeAttribute::Description
                                  | QOpcUa::NodeAttribute::DataType
                                  | QOpcUa::NodeAttribute::BrowseName
                                  | QOpcUa::NodeAttribute::DisplayName
                                  ))
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent) : TreeItem(node, model, parent)
{
    mNodeBrowseName = browsingData.browseName().name();
    mNodeClass = browsingData.nodeClass();
    mNodeId = browsingData.targetNodeId().nodeId();
    mNodeDisplayName = browsingData.displayName().text();
}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

TreeItem *TreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "TreeItem in row" << row << "does not exist.";
    return mChildItems[row];
}

int TreeItem::childIndex(const TreeItem *child) const
{
    return mChildItems.indexOf(const_cast<TreeItem *>(child));
}

int TreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

int TreeItem::columnCount() const
{
    return numberOfDisplayColumns;
}

QVariant TreeItem::data(int column)
{
    if (column == 0)
        return mNodeBrowseName;
    if (column == 1) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);

        return variantToString(value, type);
    }
    if (column == 2) {
        QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        QString name = metaEnum.valueToKey(int(mNodeClass));
        return name + " (" + QString::number(int(mNodeClass)) + ')';
    }
    if (column == 3) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const QString typeId = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        auto enumEntry = QOpcUa::namespace0IdFromNodeId(typeId);
        if (enumEntry == QOpcUa::NodeIds::Namespace0::Unknown)
            return typeId;
        return QOpcUa::namespace0IdName(enumEntry) + " (" + typeId + ")";
    }
    if (column == 4)
        return mNodeId;
    if (column == 5)
        return mNodeDisplayName;
    if (column == 6) {
        return mAttributesReady
                   ? mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUaLocalizedText>().text()
                   : tr("Loading ...");
    }
    return QVariant();
}

int TreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}

void TreeItem::appendChild(TreeItem *child)
{
    if (!child)
        return;

    if (!hasChildNodeItem(child->mNodeId)) {
        mChildItems.append(child);
        mChildNodeIds.insert(child->mNodeId);
    } else {
        child->deleteLater();
    }
}

static QPixmap createPixmap(const QColor &c)
{
    QPixmap p(10,10);
    p.fill(c);
    return p;
}

QPixmap TreeItem::icon(int column) const
{
    if (column != 0 || !mOpcNode)
        return QPixmap();

    static const QPixmap objectPixmap = createPixmap(Qt::darkGreen);
    static const QPixmap variablePixmap = createPixmap(Qt::darkBlue);
    static const QPixmap methodPixmap = createPixmap(Qt::darkRed);
    static const QPixmap defaultPixmap = createPixmap(Qt::gray);

    switch (mNodeClass) {
    case QOpcUa::NodeClass::Object:
        return objectPixmap;
    case QOpcUa::NodeClass::Variable:
        return variablePixmap;
    case QOpcUa::NodeClass::Method:
        return methodPixmap;
    default:
        break;
    }

    return defaultPixmap;
}

bool TreeItem::hasChildNodeItem(const QString &nodeId) const
{
    return mChildNodeIds.contains(nodeId);
}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void TreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUaQualifiedName>().name();
    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUaLocalizedText>().text();

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

void TreeItem::browseFinished(const QVector<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children) {
        if (hasChildNodeItem(item.targetNodeId().nodeId()))
            continue;

        auto node = mModel->opcUaClient()->node(item.targetNodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
            continue;
        }

        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + 1);
        appendChild(new TreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

QString TreeItem::variantToString(const QVariant &value, const QString &typeNodeId) const
{
    if (value.type() == QVariant::List) {
        const auto list = value.toList();
        QString concat;
        for (int i = 0, size = list.size(); i < size; ++i) {
            if (i)
                concat.append(QLatin1Char('\n'));
            concat.append(variantToString(list.at(i), typeNodeId));
        }
        return concat;
    }

    if (typeNodeId == QLatin1String("ns=0;i=19")) { // StatusCode
        const char *name = QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(value.toInt());
        return name ? QLatin1String(name) : QLatin1String("Unknown StatusCode");
    }
    if (typeNodeId == QLatin1String("ns=0;i=2")) // Char
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=3")) // SChar
        return QString::number(value.toUInt());
    if (typeNodeId == QLatin1String("ns=0;i=4")) // Int16
        return QString::number(value.toInt());
    if (typeNodeId == QLatin1String("ns=0;i=5")) // UInt16
        return QString::number(value.toUInt());
    if (value.type() == QVariant::ByteArray)
        return QLatin1String("0x") + value.toByteArray().toHex();
    if (value.type() == QVariant::DateTime)
        return value.toDateTime().toString(Qt::ISODate);
    if (value.canConvert<QOpcUaQualifiedName>()) {
        const auto name = value.value<QOpcUaQualifiedName>();
        return QStringLiteral("[NamespaceIndex: %1, Name: \"%2\"]").arg(name.namespaceIndex()).arg(name.name());
    }
    if (value.canConvert<QOpcUaLocalizedText>()) {
        const auto text = value.value<QOpcUaLocalizedText>();
        return localizedTextToString(text);
    }
    if (value.canConvert<QOpcUaRange>()) {
        const auto range = value.value<QOpcUaRange>();
        return rangeToString(range);
    }
    if (value.canConvert<QOpcUaComplexNumber>()) {
        const auto complex = value.value<QOpcUaComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUaDoubleComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    }
    if (value.canConvert<QOpcUaXValue>()) {
        const auto xv = value.value<QOpcUaXValue>();
        return QStringLiteral("[X: %1, Value: %2]").arg(xv.x()).arg(xv.value());
    }
    if (value.canConvert<QOpcUaEUInformation>()) {
        const auto info = value.value<QOpcUaEUInformation>();
        return euInformationToString(info);
    }
    if (value.canConvert<QOpcUaAxisInformation>()) {
        const auto info = value.value<QOpcUaAxisInformation>();
        return QStringLiteral("[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, AxisSteps: %5]").arg(
                                                                                                                   euInformationToString(info.engineeringUnits())).arg(rangeToString(info.eURange())).arg(localizedTextToString(info.title())).arg(
                info.axisScaleType() == QOpcUa::AxisScale::Linear ? "Linear" : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln" : "Log").arg(
                numberArrayToString(info.axisSteps()));
    }
    if (value.canConvert<QOpcUaExpandedNodeId>()) {
        const auto id = value.value<QOpcUaExpandedNodeId>();
        return QStringLiteral("[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]").arg(
                                                                                                id.nodeId()).arg(id.serverIndex()).arg(id.namespaceUri());
    }
    if (value.canConvert<QOpcUaArgument>()) {
        const auto a = value.value<QOpcUaArgument>();

        return QStringLiteral("[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", ArrayDimensions: %4, Description: %5]").arg(
                                                                                                                              a.name(), a.dataTypeId()).arg(a.valueRank()).arg(numberArrayToString(a.arrayDimensions()),
                 localizedTextToString(a.description()));
    }
    if (value.canConvert<QOpcUaExtensionObject>()) {
        const auto obj = value.value<QOpcUaExtensionObject>();
        return QStringLiteral("[TypeId: \"%1\", Encoding: %2, Body: 0x%3]").arg(obj.encodingTypeId(),
                                                                                obj.encoding() == QOpcUaExtensionObject::Encoding::NoBody ?
                                                                                    "NoBody" : (obj.encoding() == QOpcUaExtensionObject::Encoding::ByteString ?
                                                                                                                                                 "ByteString" : "XML")).arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
}

QString TreeItem::localizedTextToString(const QOpcUaLocalizedText &text) const
{
    return QStringLiteral("[Locale: \"%1\", Text: \"%2\"]").arg(text.locale()).arg(text.text());
}

QString TreeItem::rangeToString(const QOpcUaRange &range) const
{
    return QStringLiteral("[Low: %1, High: %2]").arg(range.low()).arg(range.high());
}

QString TreeItem::euInformationToString(const QOpcUaEUInformation &info) const
{
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]").arg(info.unitId()).arg(
                                                                                                                        info.namespaceUri()).arg(localizedTextToString(info.displayName())).arg(localizedTextToString(info.description()));
}

QT_END_NAMESPACE
