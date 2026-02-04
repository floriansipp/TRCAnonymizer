#include "TRCAnonymizer.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QRadioButton>
#include <QLabel>
#include <QMenu>
#include <QThread>
#include "Utility.h"
#include "AnonymizationWorker.h"
#include "LutAnonymizationWorker.h"
#include "InformationExtractionWorker.h"
#include "DuplicateCheckWorker.h"

TRCAnonymizer::TRCAnonymizer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.NameLineEdit->setMaxLength(20);
    ui.SurnameLineEdit->setMaxLength(22);
    ui.DayLineEdit->setMaxLength(2);
    ui.MonthLineEdit->setMaxLength(2);
    ui.YearLineEdit->setMaxLength(4);

    // Setup UI components
    setupMenuBar();
    setupStatusBar();

    // Connect buttons still present in the UI
    connect(ui.AddPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AddFilesToList);
    connect(ui.RemovePushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveFilesFromList);

    connect(ui.listWidget, &QListWidget::itemClicked, this, &TRCAnonymizer::OnItemSelected);
    connect(ui.listWidget, &QListWidget::currentItemChanged, this, &TRCAnonymizer::OnCurrentItemChanged);
    connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, &TRCAnonymizer::OnSelectionChanged);

    connect(ui.GenerateCSVPushButton, &QPushButton::clicked, this, &TRCAnonymizer::GenerateLookUpTableTemplate);
    connect(ui.BrowseLUTPushButton, &QPushButton::clicked, this, &TRCAnonymizer::BrowseForLookUpTable);
    connect(ui.ProcessFilesLUTPushButton, &QPushButton::clicked, this, &TRCAnonymizer::SaveLUT);

    connect(ui.CheckDuplicateNamePushButton, &QPushButton::clicked, this, &TRCAnonymizer::CheckFileDuplicate);
    connect(ui.GenerateExportCSVPushButton, &QPushButton::clicked, this, &TRCAnonymizer::ExportFileInformations);

    connect(ui.EditInfoPushButton, &QPushButton::clicked, this, &TRCAnonymizer::ToggleEditableFields);

    connect(ui.NameLineEdit, &QLineEdit::editingFinished, this, [&]{ m_eegFile->Name(ui.NameLineEdit->text().toStdString()); });
    connect(ui.SurnameLineEdit, &QLineEdit::editingFinished, this, [&]{ m_eegFile->Surname(ui.SurnameLineEdit->text().toStdString()); });
    connect(ui.DayLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int day = ui.DayLineEdit->text().toInt();
        m_eegFile->Day(day);
    });
    connect(ui.MonthLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int month = ui.MonthLineEdit->text().toInt();
        m_eegFile->Month(month);
    });
    connect(ui.YearLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int year = ui.YearLineEdit->text().toInt();
        m_eegFile->Year(year);
    });
    connect(ui.RecordDayLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int day = ui.RecordDayLineEdit->text().toInt();
        m_eegFile->RecordDay(day);
    });
    connect(ui.RecordMonthLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int month = ui.RecordMonthLineEdit->text().toInt();
        m_eegFile->RecordMonth(month);
    });
    connect(ui.RecordYearLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int year = ui.RecordYearLineEdit->text().toInt();
        m_eegFile->RecordYear(year);
    });
    connect(ui.RecordTimeHourLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int hour = ui.RecordTimeHourLineEdit->text().toInt();
        m_eegFile->RecordTimeHour(hour);
    });
    connect(ui.RecordTimeMinuteLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int minute = ui.RecordTimeMinuteLineEdit->text().toInt();
        m_eegFile->RecordTimeMinute(minute);
    });
    connect(ui.RecordTimeSecondsLineEdit, &QLineEdit::editingFinished, this, [&]
    {
        int year = ui.RecordTimeSecondsLineEdit->text().toInt();
        m_eegFile->RecordTimeSecond(year);
    });

    connect(ui.AnonHeaderPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AnonymizeHeader);
    connect(ui.CaseSensitivCheckBox, &QCheckBox::toggled, this, [&](bool isChecked){ m_researchCaseSensitiv = isChecked ? Qt::CaseSensitive : Qt::CaseInsensitive; });
    connect(ui.ReplaceGoButton, &QPushButton::clicked, this, &TRCAnonymizer::ReplaceLabelInMontages);
    connect(ui.MontagesListWidget, &QListWidget::itemChanged, this, &TRCAnonymizer::OnItemChanged);
    connect(ui.CheckAllBox, &QCheckBox::clicked, this, &TRCAnonymizer::CheckUncheckAll);
    connect(ui.RemoveMontagesPushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveSelectedMontages);
    connect(ui.SaveAnonymizedFilePushButton, &QPushButton::clicked, this, &TRCAnonymizer::SaveAnonymizedFile);

    // Notes connections
    connect(ui.NotesTableWidget, &QTableWidget::itemChanged, this, &TRCAnonymizer::OnNoteItemChanged);
    ui.NotesTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui.NotesTableWidget, &QTableWidget::customContextMenuRequested, this, &TRCAnonymizer::OnNotesContextMenu);

    // Log dock clear button
    connect(ui.clearLogPushButton, &QPushButton::clicked, this, &TRCAnonymizer::clearLog);

    // Anonymization mode UI connections
    connect(ui.FullAnonymizationRadioButton, &QRadioButton::toggled, this, [&](bool checked)
    {
        // Enable/disable LUT browse when Full Anonymization is selected
        ui.LookUpTableLineEdit->setEnabled(!checked);
        ui.BrowseLUTPushButton->setEnabled(!checked);
        // Enable/disable Recording Date Options tab
        ui.RecordingDateTab->setEnabled(checked);
    });

    connect(ui.PreserveTimelineCheckBox, &QCheckBox::toggled, this, [&](bool checked)
    {
        // Enable/disable reference date options
        ui.AutoDetectReferenceRadioButton->setEnabled(checked);
        ui.ManualReferenceRadioButton->setEnabled(checked);
        ui.ReferenceDateEdit->setEnabled(checked && ui.ManualReferenceRadioButton->isChecked());
    });

    connect(ui.ManualReferenceRadioButton, &QRadioButton::toggled, this, [&](bool checked)
    {
        // Enable date edit only when manual reference is selected
        ui.ReferenceDateEdit->setEnabled(checked && ui.PreserveTimelineCheckBox->isChecked());
    });
}

TRCAnonymizer::~TRCAnonymizer()
{
}

void TRCAnonymizer::setupMenuBar()
{
    // File menu actions
    connect(ui.actionOpen_Folder, &QAction::triggered, this, &TRCAnonymizer::LoadFolder);
    connect(ui.actionQuit, &QAction::triggered, this, &QMainWindow::close);

    // Help menu actions
    connect(ui.actionAbout_TRCAnonymizer, &QAction::triggered, this, &TRCAnonymizer::showAboutDialog);
}

void TRCAnonymizer::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    m_fileCountLabel = new QLabel("0 files staged", this);

    ui.statusbar->addWidget(m_statusLabel, 1);
    ui.statusbar->addPermanentWidget(m_fileCountLabel);
}

void TRCAnonymizer::LoadFolder()
{
    QFileDialog *fileDial = new QFileDialog(this);
    fileDial->setFileMode(QFileDialog::FileMode::AnyFile);
    fileDial->setNameFilters(QStringList()<<"*.trc" << "*.edf");
    QString fileName = fileDial->getExistingDirectory(this,  tr("Choose folder with one or multiple eeg files : "), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    if (fileName != "")
    {
        QDir currentDir = QDir(fileName);
        QStringList entries = currentDir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
        if (entries.size() > 0)
        {
            //Reset UI
            for (int i = ui.listWidget->count() - 1; i >= 0; i--)
            {
                QString label = ui.listWidget->item(i)->text();
                ui.listWidget->item(i)->~QListWidgetItem();
                m_fileMapDictionnary.remove(label);
            }
            ui.logBrowser->clear();
            ui.LookUpTableLineEdit->setText("");
            //Load new folder in UI
            LoadTreeViewUI(currentDir.absolutePath());
            updateFileCount();
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
    m_localFileSystemModel->setNameFilters(QStringList() << "*.trc" << "*.edf");
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

void TRCAnonymizer::LoadMontagesUI(std::vector<GenericMontage> montages)
{
    ui.MontagesListWidget->clear();
    for(int i = 0; i < montages.size(); i++)
    {
        QString description = QString::fromStdString(montages[i].Name());
        QListWidgetItem *currentMontage = new QListWidgetItem(ui.MontagesListWidget);
        currentMontage->setFlags(currentMontage->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        currentMontage->setFlags(currentMontage->flags() ^ Qt::ItemIsEditable); // set editable flag
        currentMontage->setCheckState(Qt::Unchecked); // AND initialize check state
        currentMontage->setText(description);
    }
}

void TRCAnonymizer::LoadNotesUI(std::vector<INote*> notes)
{
    const QSignalBlocker blocker(ui.NotesTableWidget);
    ui.NotesTableWidget->setRowCount(0);
    ui.NotesTableWidget->setRowCount(notes.size());

    int samplingRate = m_eegFile->SamplingRate();
    QTime recordStart(m_eegFile->RecordTimeHour(), m_eegFile->RecordTimeMinute(), m_eegFile->RecordTimeSecond());

    for(int i = 0; i < notes.size(); i++)
    {
        QString timeStr;
        if(samplingRate > 0)
        {
            int secondsFromStart = notes[i]->Sample() / samplingRate;
            QTime noteTime = recordStart.addSecs(secondsFromStart);
            timeStr = noteTime.toString("HH:mm:ss");
        }
        else
        {
            timeStr = QString::number(notes[i]->Sample());
        }
        QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
        timeItem->setTextAlignment(Qt::AlignCenter);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        ui.NotesTableWidget->setItem(i, 0, timeItem);

        QString description = QString::fromStdString(notes[i]->Description());
        description.remove(QChar('\0'));
        QTableWidgetItem* descItem = new QTableWidgetItem(description);
        descItem->setTextAlignment(Qt::AlignCenter);
        ui.NotesTableWidget->setItem(i, 1, descItem);
    }
    ui.NotesTableWidget->horizontalHeader()->setStretchLastSection(true);
}

QHash<std::string, std::string> TRCAnonymizer::LoadLUT(std::string path)
{
    QHash<std::string, std::string> LookUpTable;
    std::vector<std::string> rawFile = Utility::ReadTxtFile(path);
    for(int i = 0; i < rawFile.size(); i++)
    {
        std::string line = rawFile[i];

        std::vector<std::string> lineSplit = Utility::split<std::string>(line,";");
        if(lineSplit.size() == 3)
        {
            LookUpTable[lineSplit[0]] = lineSplit[1] + "#" + lineSplit[2];
        }
        else
        {
            DisplayLog("----------------------------");
            DisplayLog("Error, there should be 3 elements per line, i see " + QString::number(lineSplit.size()));
            DisplayLog("I will return an empty lookup table, please check your csv file");
            DisplayLog("----------------------------");
            return QHash<std::string, std::string>();
        }
    }
    return LookUpTable;
}

void TRCAnonymizer::EnableFieldsEdit(bool editable)
{
    ui.NameLineEdit->setEnabled(editable);
    ui.SurnameLineEdit->setEnabled(editable);
    ui.YearLineEdit->setEnabled(editable);
    ui.MonthLineEdit->setEnabled(editable);
    ui.DayLineEdit->setEnabled(editable);
    ui.RecordDayLineEdit->setEnabled(editable);
    ui.RecordMonthLineEdit->setEnabled(editable);
    ui.RecordYearLineEdit->setEnabled(editable);
//    We do not edit record time at the moment
//    if there is some demand, we will put it back with an option
//    ui.RecordTimeHourLineEdit->setEnabled(editable);
//    ui.RecordTimeMinuteLineEdit->setEnabled(editable);
//    ui.RecordTimeSecondsLineEdit->setEnabled(editable);
}

void TRCAnonymizer::DisplayLog(QString messageToDisplay)
{
    ui.logBrowser->append(messageToDisplay);
}

void TRCAnonymizer::DisplayColoredLog(QString messageToDisplay, QColor color)
{
    ui.logBrowser->setTextColor(color);
    DisplayLog(messageToDisplay);
    ui.logBrowser->setTextColor(Qt::GlobalColor::black);
}

void TRCAnonymizer::CollectFilesRecursively(const QString& dirPath, QStringList& filePaths)
{
    QDir dir(dirPath);

    // Get all files with supported extensions
    QStringList filters;
    filters << "*.trc" << "*.TRC" << "*.edf" << "*.EDF";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fileInfo : files)
    {
        filePaths.append(fileInfo.absoluteFilePath());
    }

    // Recurse into subdirectories
    QFileInfoList subdirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& subdir : subdirs)
    {
        CollectFilesRecursively(subdir.absoluteFilePath(), filePaths);
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
        QMessageBox::information(this, "Error", "You need to select at least one file or folder");
        return;
    }

    QStringList filesToAdd;

    for (int i = 0; i < selectedIndexes.size(); i++)
    {
        QFileInfo info = m_localFileSystemModel->fileInfo(selectedIndexes[i]);

        if (info.isDir())
        {
            // Recursively collect all TRC/EDF files from folder
            CollectFilesRecursively(info.absoluteFilePath(), filesToAdd);
        }
        else if (info.suffix().toLower().contains("trc") || info.suffix().toLower().contains("edf"))
        {
            // Single file selection (existing behavior)
            filesToAdd.append(info.absoluteFilePath());
        }
    }

    // Add collected files to the staging list (avoiding duplicates)
    for (const QString& filePath : filesToAdd)
    {
        QFileInfo fileInfo(filePath);
        if (!m_fileMapDictionnary.contains(fileInfo.fileName()))
        {
            m_fileMapDictionnary[fileInfo.fileName()] = filePath;

            QListWidgetItem *currentBand = new QListWidgetItem(ui.listWidget);
            currentBand->setText(fileInfo.fileName());
        }
    }
    updateFileCount();
}

void TRCAnonymizer::RemoveFilesFromList()
{
    QModelIndexList indexes = ui.listWidget->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return;

    // Find the smallest index to determine what to select after deletion
    int minIndex = indexes[0].row();
    for (const QModelIndex& idx : indexes)
    {
        if (idx.row() < minIndex)
            minIndex = idx.row();
    }

    // Delete items in reverse order to avoid index shifting issues
    for (int i = indexes.size() - 1; i >= 0; i--)
    {
        int indexToDelete = indexes[i].row();
        QString label = ui.listWidget->item(indexToDelete)->text();
        delete ui.listWidget->takeItem(indexToDelete);
        m_fileMapDictionnary.remove(label);
    }

    // Select appropriate item after deletion
    int remainingCount = ui.listWidget->count();
    if (remainingCount > 0)
    {
        // Try to select the item at the position before the first deleted item
        int newIndex = minIndex - 1;
        if (newIndex < 0)
        {
            // First item was deleted, select new first item
            newIndex = 0;
        }
        else if (newIndex >= remainingCount)
        {
            // Deleted items were at the end, select last remaining
            newIndex = remainingCount - 1;
        }
        ui.listWidget->setCurrentRow(newIndex);
        OnItemSelected(ui.listWidget->currentItem());
    }
    updateFileCount();
}

void TRCAnonymizer::OnItemSelected(QListWidgetItem* item)
{
    if(item == nullptr) return;
    QString filePath = m_fileMapDictionnary[item->text()];
    m_eegFile = IFile::Create(filePath.toStdString());
    if(m_eegFile == nullptr) return;

    EnableFieldsEdit(false);

    ui.NameLineEdit->setText(QString::fromStdString(m_eegFile->Name()));
    ui.SurnameLineEdit->setText(QString::fromStdString(m_eegFile->Surname()));
    ui.DayLineEdit->setText(QString::number(m_eegFile->Day()));
    ui.MonthLineEdit->setText(QString::number(m_eegFile->Month()));
    ui.YearLineEdit->setText(QString::number(m_eegFile->Year()));
    ui.RecordDayLineEdit->setText(QString::number(m_eegFile->RecordDay()));
    ui.RecordMonthLineEdit->setText(QString::number(m_eegFile->RecordMonth()));
    ui.RecordYearLineEdit->setText(QString::number(m_eegFile->RecordYear()));
    ui.RecordTimeHourLineEdit->setText(QString::number(m_eegFile->RecordTimeHour()));
    ui.RecordTimeMinuteLineEdit->setText(QString::number(m_eegFile->RecordTimeMinute()));
    ui.RecordTimeSecondsLineEdit->setText(QString::number(m_eegFile->RecordTimeSecond()));

    m_selectedItems = 0;
    LoadMontagesUI(m_eegFile->Montages());
    LoadNotesUI(m_eegFile->Notes());
}

void TRCAnonymizer::OnItemChanged(QListWidgetItem* item)
{
    if(item == nullptr) return;

    //Deal with the selected all checkbox
    m_selectedItems += item->checkState() == Qt::Checked ? 1 : m_selectedItems == 0 ? 0 : -1;

    {
        const QSignalBlocker blocker(ui.CheckAllBox);
        if(m_selectedItems == static_cast<int>(m_eegFile->Montages().size()))
        {
            ui.CheckAllBox->setCheckState(Qt::Checked);
        }
        else
        {
            ui.CheckAllBox->setCheckState(Qt::Unchecked);
        }
    }

    //Update the label of needed montage
    std::string str = item->text().toStdString();
    QModelIndex index = ui.MontagesListWidget->indexFromItem(item);
    //std::strncpy(m_micromedFile.Montages()[index.row()].description, str.c_str(), 64);
    m_eegFile->UpdateMontageLabel(index.row(), str);
}

void TRCAnonymizer::OnCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    OnItemSelected(current);
}

void TRCAnonymizer::OnSelectionChanged()
{
    if(ui.listWidget->selectionModel()->selectedIndexes().size() == 0)
    {
        ui.NameLineEdit->setText("");
        ui.SurnameLineEdit->setText("");
        ui.YearLineEdit->setText("");
        ui.MonthLineEdit->setText("");
        ui.DayLineEdit->setText("");
        ui.RecordDayLineEdit->setText("");
        ui.RecordMonthLineEdit->setText("");
        ui.RecordYearLineEdit->setText("");
        ui.RecordTimeHourLineEdit->setText("");
        ui.RecordTimeMinuteLineEdit->setText("");
        ui.RecordTimeSecondsLineEdit->setText("");

        ui.NameLineEdit->setEnabled(false);
        EnableFieldsEdit(false);

        ui.SearchForLineEdit->setText("");
        ui.ReplaceByLineEdit->setText("");
        ui.MontagesListWidget->clear();
        ui.NotesTableWidget->setRowCount(0);
    }
}

void TRCAnonymizer::ToggleEditableFields()
{
    bool newState = !ui.NameLineEdit->isEnabled();
    EnableFieldsEdit(newState);
}

void TRCAnonymizer::AnonymizeHeader()
{
    ui.NameLineEdit->setText("Ymous");
    emit ui.NameLineEdit->editingFinished();
    ui.SurnameLineEdit->setText("Anon");
    emit ui.SurnameLineEdit->editingFinished();
    ui.DayLineEdit->setText("1");
    emit ui.DayLineEdit->editingFinished();
    ui.MonthLineEdit->setText("1");
    emit ui.MonthLineEdit->editingFinished();
    ui.YearLineEdit->setText("1900");
    emit ui.YearLineEdit->editingFinished();
    ui.RecordDayLineEdit->setText("1");
    emit ui.RecordDayLineEdit->editingFinished();
    ui.RecordMonthLineEdit->setText("1");
    emit ui.RecordMonthLineEdit->editingFinished();
    ui.RecordYearLineEdit->setText("1900");
    emit ui.RecordYearLineEdit->editingFinished();
//    We do not edit ecord time at the moment
//    if there is some demand, we will put it back with an option
//    ui.RecordTimeHourLineEdit->setText("1");
//    emit ui.RecordTimeHourLineEdit->editingFinished();
//    ui.RecordTimeMinuteLineEdit->setText("1");
//    emit ui.RecordTimeMinuteLineEdit->editingFinished();
//    ui.RecordTimeSecondsLineEdit->setText("1");
//    emit ui.RecordTimeSecondsLineEdit->editingFinished();
}

void TRCAnonymizer::ReplaceLabelInMontages()
{
    QString search = ui.SearchForLineEdit->text();
    QString replace = ui.ReplaceByLineEdit->text();

    for (int i = 0; i < ui.MontagesListWidget->count(); i++)
    {
        QString montageText = ui.MontagesListWidget->item(i)->text();
        if(montageText.contains(search, m_researchCaseSensitiv))
        {
            montageText.replace(search, replace, m_researchCaseSensitiv);
            ui.MontagesListWidget->item(i)->setText(montageText);
        }
    }
}

void TRCAnonymizer::CheckUncheckAll(bool isChecked)
{
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
            m_eegFile->RemoveMontage(i);
        }
    }
}

void TRCAnonymizer::SaveAnonymizedFile()
{
    if(ui.listWidget->count() == 0)
    {
        QMessageBox::critical(this, "Error", "You need to add at least one file to the list");
        return;
    }

    if (!m_isAlreadyRunning)
    {
        std::vector<std::string> files;
        for (int i = 0; i < ui.listWidget->count(); i++)
        {
            files.push_back(m_fileMapDictionnary[ui.listWidget->item(i)->text()].toStdString());
        }

        auto *thread = new QThread;
        auto *worker = new AnonymizationWorker(files, ui.ProcessAllFilesCheckBox->isChecked(), m_eegFile.get());

        //=== Event update displayer
        connect(worker, &AnonymizationWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
        connect(worker, &AnonymizationWorker::sendErrorLogInfo, this, [this](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });

        connect(thread, &QThread::started, worker, [worker]{ worker->Process(); });

        //=== Event From worker and thread
        connect(worker, &AnonymizationWorker::finished, thread, &QThread::quit);
        connect(worker, &AnonymizationWorker::finished, worker, &AnonymizationWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(worker, &AnonymizationWorker::finished, this, [this]
        {
            m_isAlreadyRunning = false;
            m_statusLabel->setText("Ready");
            QMessageBox::information(this, "Success", "All files have been processed");
        });

        //=== Launch Thread and lock possible second launch
        worker->moveToThread(thread);
        thread->start();
        m_isAlreadyRunning = true;
        m_statusLabel->setText("Processing...");
    }
    else
    {
        QMessageBox::critical(this, "Anonymisation is running", "Please wait until all files have been processed");
    }
}

void TRCAnonymizer::GenerateLookUpTableTemplate()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Define the *.csv file where the template exemple will be created", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),  "CSV (*.csv)");
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if (file.open(QFile::WriteOnly))
        {
            QTextStream stream(&file);
            stream << "MicromedID;Surname;Firstname" << "\n";
            stream << "PAT_1;John;Doe";
            file.close();
        }
    }
}

void TRCAnonymizer::BrowseForLookUpTable()
{
    // Start from current LUT path if set, otherwise desktop
    QString startDir = ui.LookUpTableLineEdit->text();
    if (startDir.isEmpty() || !QFileInfo(startDir).exists())
        startDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    else
        startDir = QFileInfo(startDir).absolutePath();

    QString filePath = QFileDialog::getOpenFileName(this, "Select a csv file", startDir, "CSV Files (*.csv)");
    if (!filePath.isEmpty())
        ui.LookUpTableLineEdit->setText(filePath);
}

void TRCAnonymizer::SaveLUT()
{
    if (!m_isAlreadyRunning)
    {
        // Determine anonymization mode
        AnonymizationMode mode = ui.FullAnonymizationRadioButton->isChecked()
            ? AnonymizationMode::FullAnonymization
            : AnonymizationMode::Pseudonymization;

        // For pseudonymization, LUT is required
        QHash<std::string, std::string> LookUpTable;
        if (mode == AnonymizationMode::Pseudonymization)
        {
            QFileInfo f(ui.LookUpTableLineEdit->text());
            if (!f.suffix().contains("csv") || !f.exists())
            {
                QMessageBox::critical(this, "Error", "The file does not seem to exist or the extension is not csv");
                return;
            }
            LookUpTable = LoadLUT(f.absoluteFilePath().toStdString());
        }

        // Get files list
        std::vector<std::string> files;
        for (int i = 0; i < ui.listWidget->count(); i++)
        {
            files.push_back(m_fileMapDictionnary[ui.listWidget->item(i)->text()].toStdString());
        }

        if (files.empty())
        {
            QMessageBox::critical(this, "Error", "You need to add at least one file to the list");
            return;
        }

        // Get date options for full anonymization
        bool preserveTimeline = ui.PreserveTimelineCheckBox->isChecked();
        bool useAutoReference = ui.AutoDetectReferenceRadioButton->isChecked();
        QDate referenceDate = ui.ReferenceDateEdit->date();

        std::string noteFilter = ui.NoteFilterLineEdit->text().toStdString();

        auto *thread = new QThread;
        auto *worker = new LutAnonymizationWorker(files, LookUpTable, ui.OverwriteOriginalFilesCheckBox->isChecked(),
                                                   mode, preserveTimeline, useAutoReference, referenceDate, noteFilter);

        //=== Event update displayer
        connect(worker, &LutAnonymizationWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
        connect(worker, &LutAnonymizationWorker::sendErrorLogInfo, this, [this](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });

        connect(thread, &QThread::started, worker, [worker]{ worker->Process(); });

        //=== Event From worker and thread
        connect(worker, &LutAnonymizationWorker::finished, thread, &QThread::quit);
        connect(worker, &LutAnonymizationWorker::finished, worker, &LutAnonymizationWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(worker, &LutAnonymizationWorker::finished, this, [this]
        {
            m_isAlreadyRunning = false;
            m_statusLabel->setText("Ready");
            QMessageBox::information(this, "Success", "All files have been processed");
        });

        //=== Launch Thread and lock possible second launch
        worker->moveToThread(thread);
        thread->start();
        m_isAlreadyRunning = true;
        m_statusLabel->setText("Processing...");
    }
    else
    {
        QMessageBox::critical(this, "Anonymisation is running", "Please wait until all files have been processed");
    }
}

void TRCAnonymizer::ExportFileInformations()
{
    if (!m_isAlreadyRunning)
    {
        QString filePath = QFileDialog::getSaveFileName(this, "Choose where the *.csv file with your files informations will be created", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),  "CSV (*.csv)");
        if (!filePath.isEmpty())
        {
            std::vector<std::string> files;
            for (int i = 0; i < ui.listWidget->count(); i++)
            {
                files.push_back(m_fileMapDictionnary[ui.listWidget->item(i)->text()].toStdString());
            }

            auto *thread = new QThread;
            auto *worker = new InformationExtractionWorker(filePath, files);

            //=== Event update displayer
            connect(worker, &InformationExtractionWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
            connect(worker, &InformationExtractionWorker::sendErrorLogInfo, this, [this](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });

            connect(thread, &QThread::started, worker, [worker]{ worker->Process(); });

            //=== Event From worker and thread
            connect(worker, &InformationExtractionWorker::finished, thread, &QThread::quit);
            connect(worker, &InformationExtractionWorker::finished, worker, &InformationExtractionWorker::deleteLater);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            connect(worker, &InformationExtractionWorker::finished, this, [this]
                    {
                        m_isAlreadyRunning = false;
                        m_statusLabel->setText("Ready");
                        QMessageBox::information(this, "Success", "All files have been processed");
                    });

            //=== Launch Thread and lock possible second launch
            worker->moveToThread(thread);
            thread->start();
            m_isAlreadyRunning = true;
            m_statusLabel->setText("Exporting...");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Can't have an empty file path");
        }
    }
    else
    {
        QMessageBox::critical(this, "Process is running", "Please wait until all files have been processed");
    }
}

void TRCAnonymizer::CheckFileDuplicate()
{
    if (!m_isAlreadyRunning)
    {
        if(m_localFileSystemModel != nullptr)
        {
            QString rootDir = m_localFileSystemModel->rootPath();

            auto *thread = new QThread;
            auto *worker = new DuplicateCheckWorker(rootDir);

            //=== Event update displayer
            connect(worker, &DuplicateCheckWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
            connect(worker, &DuplicateCheckWorker::sendErrorLogInfo, this, [this](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });

            connect(thread, &QThread::started, worker, [worker]{ worker->Process(); });

            //=== Event From worker and thread
            connect(worker, &DuplicateCheckWorker::finished, thread, &QThread::quit);
            connect(worker, &DuplicateCheckWorker::finished, worker, &DuplicateCheckWorker::deleteLater);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            connect(worker, &DuplicateCheckWorker::finished, this, [this]
                    {
                        m_isAlreadyRunning = false;
                        m_statusLabel->setText("Ready");
                        QMessageBox::information(this, "Success", "All files have been checked");
                    });

            //=== Launch Thread and lock possible second launch
            worker->moveToThread(thread);
            thread->start();
            m_isAlreadyRunning = true;
            m_statusLabel->setText("Checking duplicates...");
        }
        else
        {
            QMessageBox::critical(this, "Error", "you need to load a folder with some data");
        }
    }
    else
    {
        QMessageBox::critical(this, "Process is running", "Please wait until all files have been checked");
    }
}

void TRCAnonymizer::updateFileCount()
{
    int count = ui.listWidget->count();
    QString text = QString("%1 file%2 staged").arg(count).arg(count == 1 ? "" : "s");
    if (m_fileCountLabel)
        m_fileCountLabel->setText(text);
}

void TRCAnonymizer::showAboutDialog()
{
    QMessageBox::about(this, "About TRC Anonymizer",
        "<h3>TRC Anonymizer v1.0.0</h3>"
        "<p>A tool for anonymizing TRC and EDF EEG files.</p>"
        "<p>Supports single file editing, batch processing via lookup tables, "
        "full anonymization with date shifting, and pseudonymization.</p>");
}

void TRCAnonymizer::clearLog()
{
    ui.logBrowser->clear();
}

void TRCAnonymizer::OnNoteItemChanged(QTableWidgetItem* item)
{
    if(item == nullptr || m_eegFile == nullptr) return;
    if(item->column() != 1) return;

    int row = item->row();
    std::vector<INote*> notes = m_eegFile->Notes();
    if(row >= 0 && row < notes.size())
    {
        m_eegFile->UpdateNote(row, notes[row]->Sample(), item->text().toStdString());
    }
}

void TRCAnonymizer::OnNotesContextMenu(const QPoint& pos)
{
    if(m_eegFile == nullptr) return;

    QModelIndexList selected = ui.NotesTableWidget->selectionModel()->selectedRows();
    if(selected.isEmpty()) return;

    QMenu menu(this);
    QAction* deleteAction = menu.addAction("Delete selected notes");
    QAction* chosen = menu.exec(ui.NotesTableWidget->viewport()->mapToGlobal(pos));

    if(chosen == deleteAction)
    {
        // Collect rows and sort descending to remove from the end first
        std::vector<int> rows;
        for(const QModelIndex& idx : selected)
            rows.push_back(idx.row());
        std::sort(rows.begin(), rows.end(), std::greater<int>());

        for(int row : rows)
            m_eegFile->RemoveNote(row);

        LoadNotesUI(m_eegFile->Notes());
    }
}
