#include "layerlistmodel.h"
#include <QDebug>
LayerListModel::LayerListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int LayerListModel::rowCount(const QModelIndex &parent) const
{
    return 10;
}

QVariant LayerListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return QString("asdf" + QString::number(index.row()));
    return QVariant();
}
