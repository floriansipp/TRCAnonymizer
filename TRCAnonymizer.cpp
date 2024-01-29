#include "TRCAnonymizer.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <stdio.h>
#include <string.h>
#include "Utility.h"
#include "EdfFile.h"
#include "MicromedFile.h"

TRCAnonymizer::TRCAnonymizer(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.NameLineEdit->setMaxLength(20);
    ui.SurnameLineEdit->setMaxLength(22);
    ui.DayLineEdit->setMaxLength(2);
    ui.MonthLineEdit->setMaxLength(2);
    ui.YearLineEdit->setMaxLength(4);

    connect(ui.LoadFolderPushButton, &QPushButton::clicked, this, &TRCAnonymizer::LoadFolder);
    connect(ui.AddPushButton, &QPushButton::clicked, this, &TRCAnonymizer::AddFilesToList);
    connect(ui.RemovePushButton, &QPushButton::clicked, this, &TRCAnonymizer::RemoveFilesFromList);

    connect(ui.listWidget, &QListWidget::itemClicked, this, &TRCAnonymizer::OnItemSelected);
    connect(ui.listWidget, &QListWidget::currentItemChanged, this, &TRCAnonymizer::OnCurrentItemChanged);
    connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, &TRCAnonymizer::OnSelectionChanged);

    //connect(ui.ProcessAllFilesCheckBox, &QCheckBox::clicked, this, [&](bool b){ });
    //connect(ui.LookUpTableLineEdit, &QLineEdit::editingFinished, this, [&]{ qDebug() << "New Text : " << ui.LookUpTableLineEdit->text(); });
    connect(ui.GenerateCSVPushButton, &QPushButton::clicked, this, [&]
    {
        QString filePath = QFileDialog::getSaveFileName(this, "Define the *.csv file where the template exemple will be created", QDir::homePath(),  "CSV (*.csv)");
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
    });
    connect(ui.BrowseLUTPushButton, &QPushButton::clicked, this, [&]
    {
        QString filePath = QFileDialog::getOpenFileName(this, "Select a csv file", "", "*.csv");
        if (!filePath.isEmpty())
            ui.LookUpTableLineEdit->setText(filePath);
    });
    connect(ui.ProcessFilesLUTPushButton, &QPushButton::clicked, this, &TRCAnonymizer::SaveLUT);

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
        int year = ui.YearLineEdit->text().toInt() - 1900;
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
        int year = ui.RecordYearLineEdit->text().toInt() - 1900;
        m_eegFile->RecordYear(year);
;    });
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
}

TRCAnonymizer::~TRCAnonymizer()
{
    Utility::DeleteAndNullify(m_eegFile);
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
            ui.MessageDisplayer->clear();
            ui.LookUpTableLineEdit->setText("");
            //Load new folder in UI
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
//    We do not edit record day and record time at the moment
//    if there is some demand, we will put it back with an option
//    ui.RecordDayLineEdit->setEnabled(editable);
//    ui.RecordMonthLineEdit->setEnabled(editable);
//    ui.RecordYearLineEdit->setEnabled(editable);
//    ui.RecordTimeHourLineEdit->setEnabled(editable);
//    ui.RecordTimeMinuteLineEdit->setEnabled(editable);
//    ui.RecordTimeSecondsLineEdit->setEnabled(editable);
}

IFile* TRCAnonymizer::LoadEegFile(QString filepath)
{
    QFileInfo f(filepath);
    if(f.suffix().toLower().contains("trc"))
    {
        return new MicromedFile(filepath.toStdString());
    }
    else if(f.suffix().toLower().contains("edf"))
    {
        return new EdfFile(filepath.toStdString());
    }
    else
    {
        return nullptr;
    }
}

void TRCAnonymizer::DisplayLog(QString messageToDisplay)
{
    ui.MessageDisplayer->append(messageToDisplay);
}

void TRCAnonymizer::DisplayColoredLog(QString messageToDisplay, QColor color)
{
    ui.MessageDisplayer->setTextColor(color);
    DisplayLog(messageToDisplay);
    ui.MessageDisplayer->setTextColor(Qt::GlobalColor::black);
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
        if(info.suffix().toLower().contains("trc") || info.suffix().toLower().contains("edf"))
        {
            if(!m_fileMapDictionnary.contains(info.fileName()))
            {
                m_fileMapDictionnary[info.fileName()] = info.absoluteFilePath();

                QListWidgetItem *currentBand = new QListWidgetItem(ui.listWidget);
                currentBand->setText(info.fileName());
            }
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
    if(item == nullptr) return;
    QString filePath = m_fileMapDictionnary[item->text()];
    Utility::DeleteAndNullify(m_eegFile);
    m_eegFile = LoadEegFile(filePath);
    if(m_eegFile == nullptr) return;

    ui.NameLineEdit->setText(QString::fromStdString(m_eegFile->Name()));
    ui.SurnameLineEdit->setText(QString::fromStdString(m_eegFile->Surname()));
    ui.DayLineEdit->setText(QString::number(m_eegFile->Day()));
    ui.MonthLineEdit->setText(QString::number(m_eegFile->Month()));
    ui.YearLineEdit->setText(QString::number(1900 + m_eegFile->Year()));
    ui.RecordDayLineEdit->setText(QString::number(m_eegFile->RecordDay()));
    ui.RecordMonthLineEdit->setText(QString::number(m_eegFile->RecordMonth()));
    ui.RecordYearLineEdit->setText(QString::number(1900 + m_eegFile->RecordYear()));
    ui.RecordTimeHourLineEdit->setText(QString::number(m_eegFile->RecordTimeHour()));
    ui.RecordTimeMinuteLineEdit->setText(QString::number(m_eegFile->RecordTimeMinute()));
    ui.RecordTimeSecondsLineEdit->setText(QString::number(m_eegFile->RecordTimeSecond()));

    m_selectedItems = 0;
    LoadMontagesUI(m_eegFile->Montages());
}

void TRCAnonymizer::OnItemChanged(QListWidgetItem* item)
{
    if(item == nullptr) return;

    //Deal with the selected all checkbox
    m_selectedItems += item->checkState() == Qt::Checked ? 1 : m_selectedItems == 0 ? 0 : -1;

    m_lock = true;
    if(m_selectedItems == m_eegFile->Montages().size())
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
//    We do not edit record day and record time at the moment
//    if there is some demand, we will put it back with an option
//    ui.RecordDayLineEdit->setText("1");
//    emit ui.RecordDayLineEdit->editingFinished();
//    ui.RecordMonthLineEdit->setText("1");
//    emit ui.RecordMonthLineEdit->editingFinished();
//    ui.RecordYearLineEdit->setText("1900");
//    emit ui.RecordYearLineEdit->editingFinished();
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

        thread = new QThread;
        worker = new AnonymizationWorker(files, ui.ProcessAllFilesCheckBox->isChecked(), m_eegFile);

        //=== Event update displayer
        connect(worker, &AnonymizationWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
        connect(worker, &AnonymizationWorker::sendErrorLogInfo, this, [&](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });
        connect(worker, &AnonymizationWorker::progress, this, [&](double d) { });

        connect(thread, &QThread::started, this, [&]{ worker->Process(); });

        //=== Event From worker and thread
        connect(worker, &AnonymizationWorker::finished, thread, &QThread::quit);
        connect(worker, &AnonymizationWorker::finished, worker, &AnonymizationWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        connect(worker, &AnonymizationWorker::finished, this, [&]
        {
            m_isAlreadyRunning = false;
            QMessageBox::information(this, "Success", "All files have been processed");
            /*mettre avancement à 100%*/
        });

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

void TRCAnonymizer::SaveLUT()
{
    if (!m_isAlreadyRunning)
    {
        QFileInfo f(ui.LookUpTableLineEdit->text());
        if(f.suffix().contains("csv") && f.exists())
        {
            std::vector<std::string> files;
            for (int i = 0; i < ui.listWidget->count(); i++)
            {
                files.push_back(m_fileMapDictionnary[ui.listWidget->item(i)->text()].toStdString());
            }

            QHash<std::string, std::string> LookUpTable = LoadLUT(f.absoluteFilePath().toStdString());

            thread = new QThread;
            worker2 = new LutAnonymizationWorker(files, LookUpTable, ui.OverwriteOriginalFilesCheckBox->isChecked());

            //=== Event update displayer
            connect(worker2, &LutAnonymizationWorker::sendLogInfo, this, &TRCAnonymizer::DisplayLog);
            connect(worker2, &LutAnonymizationWorker::sendErrorLogInfo, this, [&](QString s){ DisplayColoredLog(s, Qt::GlobalColor::red); });
            connect(worker2, &LutAnonymizationWorker::progress, this, [&](double d) { });

            connect(thread, &QThread::started, this, [&]{ worker2->Process(); });

            //=== Event From worker and thread
            connect(worker2, &LutAnonymizationWorker::finished, thread, &QThread::quit);
            connect(worker2, &LutAnonymizationWorker::finished, worker2, &LutAnonymizationWorker::deleteLater);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            connect(worker2, &LutAnonymizationWorker::finished, this, [&]
            {
                m_isAlreadyRunning = false;
                /*mettre avancement à 100%*/
                QMessageBox::information(this, "Success", "All files have been processed");
            });

            //=== Launch Thread and lock possible second launch
            worker2->moveToThread(thread);
            thread->start();
            m_isAlreadyRunning = true;
        }
        else
        {
            QMessageBox::critical(this, "Error", "The file does not seem to exist or the extension is not csv");
        }
    }
    else
    {
        QMessageBox::critical(this, "Anonymisation is running", "Please wait until all files have been processed");
    }
}
