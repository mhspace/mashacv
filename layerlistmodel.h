#ifndef LAYERLISTMODEL_H
#define LAYERLISTMODEL_H

#include <QAbstractListModel>
#include <QVariant>

class LayerListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit LayerListModel(QObject *parent = 0);
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    
signals:
    
public slots:
    
};

#endif // LAYERLISTMODEL_H
