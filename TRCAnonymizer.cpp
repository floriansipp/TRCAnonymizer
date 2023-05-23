#include "TRCAnonymizer.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

TRCAnonymizer::TRCAnonymizer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    QAction* openBIDSFolder = ui.menuFiles->actions().at(0);
    connect(openBIDSFolder, &QAction::triggered, this, &TRCAnonymizer::LoadFolder);

    connect(ui.AddPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AddFilesToList);
    connect(ui.RemovePushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveFilesFromList);

    connect(ui.listWidget, &QListWidget::itemClicked, this, &TRCAnonymizer::OnItemSelected);
    connect(ui.listWidget, &QListWidget::currentItemChanged, this, &TRCAnonymizer::OnCurrentItemChanged);

    connect(ui.MontagesListWidget, &QListWidget::itemChanged, this, &TRCAnonymizer::OnItemChanged);
    connect(ui.AnonHeaderPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AnonymizeHeader);
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
        currentMontage->setCheckState(Qt::Unchecked); // AND initialize check state
        currentMontage->setText(description);
    }
}

void TRCAnonymizer::AddFilesToList()
{
    QModelIndexList selectedIndexes = ui.treeView->selectionModel()->selectedRows();
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
}

void TRCAnonymizer::OnCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    OnItemSelected(current);
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
    QFile::copy(QString::fromStdString(m_micromedFile.FilePath()), QString::fromStdString(m_micromedFile.AnonFilePath()));
    m_micromedFile.SaveAnonymizedData();
}
