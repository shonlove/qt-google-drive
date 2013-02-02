#ifndef FILESUI_H
#define FILESUI_H

#include "core/driveengine.h"
#include "share/enums.h"
#include <QLabel>

class ContentUI: public QObject
{
    Q_OBJECT
public: 
    enum EPath
    {
        EForward,
        EBackward
    };

    explicit ContentUI(QObject *parent = 0);

public:
    QLabel* getPanelLabel(EPanels panel) const;
    QList<int> getMarkedItemIds(QTreeWidget *treeWidget) const;
    void markItem(QTreeWidgetItem *item, bool noSwitch = false);
    void markItems(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    bool hasItemParentSign(QTreeWidgetItem *item) const;

private slots:
    void slotItemLeftPressed(QTreeWidgetItem *item, int column);
    void slotItemRightPressed(QTreeWidgetItem *item, int column);
    void slotLeftPanelItemDoubleClicked(QTreeWidgetItem *item, int column);
    void slotRightPanelItemDoubleClicked(QTreeWidgetItem *item, int column);
    void slotLeftCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void slotRightCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void slotUpdateFileList(void);

private:
    int getDiscLength(EPanels panel) const;
    void setPanelDisplayingPath(const QString &name, EPath path, EPanels panel);
    void showFilesOnPanel(QTreeWidgetItem *item, EPanels panel);
    void setCurrentPanelState(EPanels panel, const QString &url);
    void performShowFiles(const QString &query, const QString &name, EPath path, EPanels panel);
    bool isFolder(void);
    int itemIndex(QTreeWidgetItem *item);

private:
    Children children;
};

#endif // FILESUI_H
