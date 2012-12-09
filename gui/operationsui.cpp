#include "operationsui.h"
#include "share/debug.h"

OperationsUI::OperationsUI(QObject *parent) :
    QObject(parent),
    createFolderDialog(NULL)
{
}

void OperationsUI::del(QObject* object)
{
//    if (object == SUi::inst()->treeFoldersView)
//    {
//        if(!SDriveEngine::inst()->elStates[EAFoldersViewFocused])
//        {
//            ItemInfo item = *SDriveEngine::inst()->foldersMngr->getParser()->getXMLHandler()->getItemInfo();

//            if(item[SDriveEngine::inst()->foldersUI->getCurrFolderItemId()].parent != "")
//            {
//                SDriveEngine::inst()->getFilesMngr()->deleteFile(item[SDriveEngine::inst()->foldersUI->getCurrFolderItemId()].self);
//                delItemInTree(item);
//            }
//        }
//    }

    if (object == SDriveEngine::inst()->getFilesMngr()->getPanel())
    {
        FilesManager* manager;

//        if(SDriveEngine::inst()->elStates[EAFoldersViewFocused])
//        {
//            manager = SDriveEngine::inst()->aFoldersMngr.data();
//        }
//        else
//        {
            manager = SDriveEngine::inst()->getFilesMngr();
//        }

        //QList<ItemInfo::Data> itemData = manager->getParser()->getXMLHandler()->getItemInfo()->getFileItems();

        manager->deleteFile(SDriveEngine::inst()->getFilesMngr()->getCurrentFileInfo().self);
    }
}

//void OperationsUI::delItemInTree(ItemInfo item)
//{
//    QTreeWidgetItem *parent = item[SDriveEngine::inst()->foldersUI->getCurrFolderItemId()].pointer->parent();
//    int index;

//    if (parent)
//    {
//        index = parent->indexOfChild(SUi::inst()->treeFoldersView->currentItem());
//        delete parent->takeChild(index);
//    }
//    else
//    {
//        index = SUi::inst()->treeFoldersView->indexOfTopLevelItem(SUi::inst()->treeFoldersView->currentItem());
//        delete SUi::inst()->treeFoldersView->takeTopLevelItem(index);
//    }
//}

void OperationsUI::slotTriggeredDel(void)
{
//    if(SDriveEngine::inst()->elStates[ETrashFocused]) return;
//    if(SDriveEngine::inst()->elStates[EAFoldersViewFocused] && !SDriveEngine::inst()->elStates[ERightViewFocused]) return;

    QObject* object;

//    if(SDriveEngine::inst()->elStates[EFoldersTreeViewFocused])
//    {
//        object = SUi::inst()->treeFoldersView;
//    }
//    else
//    {
        object = SDriveEngine::inst()->getFilesMngr()->getPanel();
//    }

    del(object);
}

bool OperationsUI::operationPossible(void)
{
    bool is = false;

    int index = SDriveEngine::inst()->getFilesMngr()->getPanel()->currentIndex().row();

    if(index >= 0)
    {
        QString itemText = SDriveEngine::inst()->getFilesMngr()->getPanel()->currentItem()->text(0);

        if(itemText != PARENT_FOLDER_SIGN)
        {
            is = true;
        }
    }

    return is;
}

void OperationsUI::slotNewFolder(void)
{
    createFolderDialog = new CreateFolderDialog(SDriveEngine::inst()->parent);

    connect(createFolderDialog, SIGNAL(signalAccept(const QString&)), this, SLOT(slotAcceptCreateFolder(const QString&)));
    connect(createFolderDialog, SIGNAL(signalReject()), this, SLOT(slotRejectCreateFolder()));
    connect(createFolderDialog, SIGNAL(signalFinished(int)), this, SLOT(slotFinishedCreateFolder(int)));

    createFolderDialog->exec();
}

void OperationsUI::slotCopyWebFile(void)
{
    if(!operationPossible())
    {
        CommonTools::msg(tr("No Files selected"));
        return;
    }

    ItemInfo::Data source = SDriveEngine::inst()->getFilesMngr()->getCurrentFileInfo();
    SDriveEngine::inst()->getFilesMngr()->copyWebFile(source, SDriveEngine::inst()->getFilesMngr(true)->getUpLevelFolderLink());
}

void OperationsUI::slotMoveWebFile(void)
{
    if(!operationPossible())
    {
        CommonTools::msg(tr("No Files selected"));
        return;
    }

    ItemInfo::Data source = SDriveEngine::inst()->getFilesMngr()->getCurrentFileInfo();
    SDriveEngine::inst()->getFilesMngr()->moveWebFile(source, SDriveEngine::inst()->getFilesMngr(true)->getUpLevelFolderLink());
}

void OperationsUI::slotRenameWebFile(void)
{
    if(!operationPossible())
    {
        CommonTools::msg(tr("No Files selected"));
        return;
    }

    QTreeWidgetItem *item = SDriveEngine::inst()->getFilesMngr()->getPanel()->currentItem();

    editingItemText = item->text(0);

    item->setFlags(item->flags() | Qt::ItemIsEditable);
    SDriveEngine::inst()->getFilesMngr()->getPanel()->editItem(item, 0);

    connect(SDriveEngine::inst()->getFilesMngr()->getPanel()->itemDelegate(), SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this, SLOT(slotItemEditDone()));
}

void OperationsUI::slotItemEditDone(void)
{
    QTreeWidgetItem *item = SDriveEngine::inst()->getFilesMngr()->getPanel()->currentItem();
    ItemInfo::Data source = SDriveEngine::inst()->getFilesMngr()->getCurrentFileInfo();

    QString itemTextAfterEditing = item->text(0);

    if(editingItemText != itemTextAfterEditing)
    {
        SDriveEngine::inst()->getFilesMngr()->renameWebFile(source, itemTextAfterEditing);
        editingItemText.clear();
    }
}

void OperationsUI::slotShareWebFile(void)
{
    ItemInfo::Data source = SDriveEngine::inst()->getFilesMngr()->getCurrentFileInfo();
    SDriveEngine::inst()->getFilesMngr()->shareWebFile(source);
}

void OperationsUI::slotAcceptCreateFolder(const QString &name)
{
    createFolder(name);
}

void OperationsUI::slotRejectCreateFolder(void)
{
    delete createFolderDialog;
}

void OperationsUI::slotFinishedCreateFolder(int result)
{
    Q_UNUSED(result);
    delete createFolderDialog;
}

void OperationsUI::createFolder(const QString &name)
{   
    if(name == "" || name.contains(QRegExp("[/.<>]")) || name.contains(QRegExp("\\\\")) || name.contains(QRegExp("\"")))
    {
        CommonTools::msg("Please enter a valid name");
        return;
    }

    SDriveEngine::inst()->foldersMngr->createFolder(SDriveEngine::inst()->getFilesMngr()->getUpLevelFolderLink(), name);

    //delete createFolderDialog;
}
