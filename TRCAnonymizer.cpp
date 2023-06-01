#include "TRCAnonymizer.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <stdio.h>
#include <string.h>

TRCAnonymizer::TRCAnonymizer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.NameLineEdit->setMaxLength(20);
    ui.SurnameLineEdit->setMaxLength(22);
    ui.DayLineEdit->setMaxLength(1);
    ui.MonthLineEdit->setMaxLength(1);
    ui.YearLineEdit->setMaxLength(1);

    connect(ui.LoadFolderPushButton, &QPushButton::clicked, this, &TRCAnonymizer::LoadFolder);
    connect(ui.AddPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AddFilesToList);
    connect(ui.RemovePushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveFilesFromList);

    connect(ui.listWidget, &QListWidget::itemClicked, this, &TRCAnonymizer::OnItemSelected);
    connect(ui.listWidget, &QListWidget::currentItemChanged, this, &TRCAnonymizer::OnCurrentItemChanged);

    //connect(ui.ProcessAllFilesCheckBox, &QCheckBox::clicked, this, [&](bool b){ });
    //connect(ui.LookUpTableLineEdit, &QLineEdit::editingFinished, this, [&]{ qDebug() << "New Text : " << ui.LookUpTableLineEdit->text(); });
    connect(ui.BrowseLUTPushButton, &QPushButton::clicked, this, [&]
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Select a csv file", "", "*.csv");
        if (!filePath.isEmpty())
            ui.LookUpTableLineEdit->setText(filePath);
    });

    connect(ui.EditInfoPushButton, &QPushButton::clicked, this, &TRCAnonymizer::ToggleEditableFields);
    connect(ui.AnonHeaderPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AnonymizeHeader);
    connect(ui.MontagesListWidget, &QListWidget::itemChanged, this, &TRCAnonymizer::OnItemChanged);
    connect(ui.CheckAllBox, &QCheckBox::clicked, this, &TRCAnonymizer::CheckUncheckAll);
    connect(ui.RemoveMontagesPushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveSelectedMontages);
    connect(ui.SaveAnonymizedFilePushButton, &QPushButton::clicked, this, &TRCAnonymizer::SaveAnonymizedFile);
}

TRCAnonymizer::~TRCAnonymizer()
{

}

void TRCAnonymizer::LoadFolder()
{
    QFileDialog *fileDial = new QFileDialog(this);
    fileDial->setFileMode(QFileDialog::FileMode::AnyFile);
    fileDial->setNameFilters(QStringList()<<"*.trc");
    QString fileName = fileDial->getExistingDirectory(this,  tr("Choose folder with one or multiple eeg files : "), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (fileName != "")
    {
        QDir currentDir = QDir(fileName);
        QStringList entries = currentDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
        if (entries.size() > 0)
        {
            LoadTreeViewUI(currentDir.absolutePath());
        }
        else
        {
            QMessageBox::information(this, "Are you sure ? ", "There is no file in this folder.");
        }
    }
}

void TRCAnonymizer::LoadTreeViewUI(QString initialFolder)
{
    //Define file system model at the root folder chosen by the user
    m_localFileSystemModel = new QFileSystemModel();
    m_localFileSystemModel->setReadOnly(true);
    m_localFileSystemModel->setRootPath(initialFolder);

    //set filters
    m_localFileSystemModel->setNameFilters(QStringList() << "*.trc");
    //set model in treeview
    ui.treeView->setModel(m_localFileSystemModel);
    //Show only what is under this path
    ui.treeView->setRootIndex(m_localFileSystemModel->index(initialFolder));
    //Show everything put starts at the given model index
    //ui.FileTreeView->setCurrentIndex(m_localFileSystemModel.index(initialFolder));

    //==[Ui Layout]
    ui.treeView->setAnimated(false);
    ui.treeView->setIndentation(20);
    //Sorting enabled puts elements in reverse (last is first, first is last)
    //ui.FileTreeView->setSortingEnabled(true);
    //Hide name, file size, file type , etc
    ui.treeView->hideColumn(1);
    ui.treeView->hideColumn(2);
    ui.treeView->hideColumn(3);
    ui.treeView->header()->hide();
}

void TRCAnonymizer::LoadMontagesUI(std::vector<montagesOfTrace> montages)
{
    ui.MontagesListWidget->clear();
    for(int i = 0; i < montages.size(); i++)
    {
        QString description = QString::fromStdString(std::string(montages[i].description));
        QListWidgetItem *currentMontage = new QListWidgetItem(ui.MontagesListWidget);
        currentMontage->setFlags(currentMontage->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        currentMontage->setFlags(currentMontage->flags() ^ Qt::ItemIsEditable); // set editable flag
        currentMontage->setCheckState(Qt::Unchecked); // AND initialize check state
        currentMontage->setText(description);
    }
}

void TRCAnonymizer::AddFilesToList()
{
    if(ui.treeView->selectionModel() == nullptr)
    {
        QMessageBox::information(this, "Error", "You first need to load a folder with TRC Files in it");
        return;
    }

    QModelIndexList selectedIndexes = ui.treeView->selectionModel()->selectedRows();
    if(selectedIndexes.size() == 0)
    {
        QMessageBox::information(this, "Error", "You need to select at least one file to put in the list");
        return;
    }

    for (int i = 0; i < selectedIndexes.size(); i++)
    {
        QFileInfo info = m_localFileSystemModel->fileInfo(selectedIndexes[i]);
        if(!m_fileMapDictionnary.contains(info.fileName()))
        {
            m_fileMapDictionnary[info.fileName()] = info.absoluteFilePath();

            QListWidgetItem *currentBand = new QListWidgetItem(ui.listWidget);
            currentBand->setText(info.fileName());
        }
    }
}

void TRCAnonymizer::RemoveFilesFromList()
{
    QModelIndexList indexes = ui.listWidget->selectionModel()->selectedIndexes();
    for (int i = indexes.size() - 1; i >= 0; i--)
    {
        int indexToDelete = indexes[i].row();
        QString label = ui.listWidget->item(indexToDelete)->text();
        ui.listWidget->item(indexToDelete)->~QListWidgetItem();
        m_fileMapDictionnary.remove(label);
    }
}

void TRCAnonymizer::OnItemSelected(QListWidgetItem* item)
{
    QString filePath = m_fileMapDictionnary[item->text()];

    m_micromedFile = MicromedFile(filePath.toStdString());

    ui.NameLineEdit->setText(QString::fromStdString(m_micromedFile.Name()));
    ui.SurnameLineEdit->setText(QString::fromStdString(m_micromedFile.Surname()));
    ui.DayLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Day())));
    ui.MonthLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Month())));
    ui.YearLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Year())));

    m_selectedItems = 0;
    LoadMontagesUI(m_micromedFile.Montages());
}

void TRCAnonymizer::OnItemChanged(QListWidgetItem* item)
{
    //Deal with the selected all checkbox
    m_selectedItems += item->checkState() == Qt::Checked ? 1 : m_selectedItems == 0 ? 0 : -1;

    m_lock = true;
    if(m_selectedItems == m_micromedFile.Montages().size())
    {
        ui.CheckAllBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui.CheckAllBox->setCheckState(Qt::Unchecked);
    }
    m_lock = false;

    //Update the label of needed montage
    std::string str = item->text().toStdString();
    QModelIndex index = ui.MontagesListWidget->indexFromItem(item);
    std::strncpy(m_micromedFile.Montages()[index.row()].description, str.c_str(), 64);
}

void TRCAnonymizer::OnCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    OnItemSelected(current);
}

void TRCAnonymizer::ToggleEditableFields()
{
    bool newState = !ui.NameLineEdit->isEnabled();

    ui.NameLineEdit->setEnabled(newState);
    ui.SurnameLineEdit->setEnabled(newState);
    ui.YearLineEdit->setEnabled(newState);
    ui.MonthLineEdit->setEnabled(newState);
    ui.DayLineEdit->setEnabled(newState);
}

void TRCAnonymizer::AnonymizeHeader()
{
    m_micromedFile.AnonymizeHeaderData();

    ui.NameLineEdit->setText(QString::fromStdString(m_micromedFile.Name()));
    ui.SurnameLineEdit->setText(QString::fromStdString(m_micromedFile.Surname()));
    ui.DayLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Day())));
    ui.MonthLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Month())));
    ui.YearLineEdit->setText(QString::number(static_cast<int>(m_micromedFile.Year())));
}

void TRCAnonymizer::CheckUncheckAll(bool isChecked)
{
    if(m_lock) return;

    for (int i = 0; i < ui.MontagesListWidget->count(); i++)
    {
        Qt::CheckState state = isChecked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
        ui.MontagesListWidget->item(i)->setCheckState(state);
    }
}

void TRCAnonymizer::RemoveSelectedMontages()
{
    for (int i = ui.MontagesListWidget->count() - 1; i >= 0; i--)
    {
        if(ui.MontagesListWidget->item(i)->checkState() == Qt::CheckState::Checked)
        {
            ui.MontagesListWidget->item(i)->~QListWidgetItem();
            m_micromedFile.Montages().erase(m_micromedFile.Montages().begin() + i);
        }
    }
}

void TRCAnonymizer::SaveAnonymizedFile()
{
    if (!m_isAlreadyRunning)
    {
        std::vector<std::string> files;
        for (int i = 0; i < ui.listWidget->count(); i++)
        {
            files.push_back(m_fileMapDictionnary[ui.listWidget->item(i)->text()].toStdString());
        }

        thread = new QThread;
        worker = new AnonymizationWorker(files, ui.ProcessAllFilesCheckBox->isChecked(), &m_micromedFile);

        //=== Event update displayer
        connect(worker, &AnonymizationWorker::sendLogInfo, this, [&](QString s){ });
        connect(worker, &AnonymizationWorker::progress, this, [&](double d) { });

        connect(thread, &QThread::started, this, [&]{ worker->Process(); });

        //=== Event From worker and thread
        connect(worker, &AnonymizationWorker::finished, thread, &QThread::quit);
        connect(worker, &AnonymizationWorker::finished, worker, &AnonymizationWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(worker, &AnonymizationWorker::finished, this, [&]{ m_isAlreadyRunning = false; /*mettre avancement à 100%*/ });

        //=== Launch Thread and lock possible second launch
        worker->moveToThread(thread);
        thread->start();
        m_isAlreadyRunning = true;
    }
    else
    {
        QMessageBox::critical(this, "Anonymisation is running", "Please wait until all files have been processed");
    }
}
