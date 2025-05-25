// File: main.cpp
// Qt lazy-loading file tree model, adapted for QML

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFileInfo>
#include <QDir>
#include <QAbstractItemModel>
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <QVector>
#include <QDebug>

class FileItem {
public:
    FileItem(const QFileInfo &info, FileItem *parent = nullptr)
        : fileInfo(info), parentItem(parent), childrenLoaded(false) {}

    ~FileItem() {
        qDeleteAll(children);
    }

    void loadChildren() {
        if (childrenLoaded || !fileInfo.isDir()) return;

        QDir dir(fileInfo.absoluteFilePath());
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries, QDir::Name);

        for (const QFileInfo &entry : entries) {
            children.append(new FileItem(entry, this));
        }
        childrenLoaded = true;
    }

    FileItem *child(int row) {
        loadChildren();
        return row >= 0 && row < children.size() ? children[row] : nullptr;
    }

    int childCount() {
        loadChildren();
        return children.size();
    }

    int row() const {
        return parentItem ? parentItem->children.indexOf(const_cast<FileItem*>(this)) : 0;
    }

    QVariant data(int role) const {
        switch (role) {
        case Qt::DisplayRole:
        case Qt::UserRole + 1: return fileInfo.fileName();
        case Qt::UserRole + 2: return fileInfo.isDir() ? QStringLiteral("<DIR>") : QString::number(fileInfo.size());
        case Qt::UserRole + 3: return fileInfo.suffix();
        case Qt::UserRole + 4: return fileInfo.lastModified().toString("yyyy-MM-dd hh:mm");
        default: return QVariant();
        }
    }

    FileItem *parent() const { return parentItem; }

private:
    QFileInfo fileInfo;
    FileItem *parentItem;
    QVector<FileItem*> children;
    bool childrenLoaded;
};

class FileModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        SizeRole,
        TypeRole,
        ModifiedRole
    };

    Q_ENUM(Roles)

    FileModel(const QString &rootPath, QObject *parent = nullptr)
        : QAbstractItemModel(parent) {
        rootItem = new FileItem(QFileInfo(rootPath));
    }

    ~FileModel() override {
        delete rootItem;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        FileItem *parentItem = parent.isValid() ? static_cast<FileItem*>(parent.internalPointer()) : rootItem;
        FileItem *childItem = parentItem->child(row);
        return childItem ? createIndex(row, column, childItem) : QModelIndex();
    }

    QModelIndex parent(const QModelIndex &index) const override {
        if (!index.isValid()) return QModelIndex();

        FileItem *childItem = static_cast<FileItem*>(index.internalPointer());
        FileItem *parentItem = childItem->parent();

        if (!parentItem || parentItem == rootItem) return QModelIndex();
        return createIndex(parentItem->row(), 0, parentItem);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        FileItem *parentItem = parent.isValid() ? static_cast<FileItem*>(parent.internalPointer()) : rootItem;
        return parentItem->childCount();
    }

    int columnCount(const QModelIndex &) const override {
        return 1; // Only 1 column needed in QML TreeView
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid()) return QVariant();
        FileItem *item = static_cast<FileItem*>(index.internalPointer());
        return item->data(role);
    }

    QHash<int, QByteArray> roleNames() const override {
        return {
            { NameRole, "name" },
            { SizeRole, "size" },
            { TypeRole, "type" },
            { ModifiedRole, "modified" }
        };
    }

private:
    FileItem *rootItem;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QString safeRootPath = QDir::homePath();
    qmlRegisterSingletonInstance("FileBrowser", 1, 0, "FileModelInstance", new FileModel(safeRootPath));

    engine.loadFromModule("LazyFileBrowserQML", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
