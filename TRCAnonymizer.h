#ifndef TRCANONYMIZER_H
#define TRCANONYMIZER_H

#include "ui_TRCAnonymizer.h"
#include <QtWidgets/QMainWindow>
#include <QFileSystemModel>
#include <QHash>
#include "AnonymizationWorker.h"
#include "LutAnonymizationWorker.h"
#include "InformationExtractionWorker.h"
#include "DuplicateCheckWorker.h"
#include <QThread>
#include "IFile.h"

class QLabel;

class TRCAnonymizer : public QMainWindow
{
	Q_OBJECT

public:
    TRCAnonymizer(QWidget *parent = nullptr);
	~TRCAnonymizer();

private:
    void LoadFolder();
    void LoadTreeViewUI(QString initialFolder);
    void LoadMontagesUI(std::vector<GenericMontage> montages);
    void LoadNotesUI(std::vector<INote*> notes);
    QHash<std::string, std::string> LoadLUT(std::string path);
    void EnableFieldsEdit(bool editable);
    IFile* LoadEegFile(QString filepath);
    void CollectFilesRecursively(const QString& dirPath, QStringList& filePaths);

    void setupMenuBar();
    void setupStatusBar();

private slots:
    void DisplayLog(QString messageToDisplay);
    void DisplayColoredLog(QString messageToDisplay, QColor color);
    void AddFilesToList();
    void RemoveFilesFromList();
    void OnItemSelected(QListWidgetItem* item);
    void OnItemChanged(QListWidgetItem* item);
    void OnCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void OnSelectionChanged();
    void ToggleEditableFields();
    void AnonymizeHeader();
    void ReplaceLabelInMontages();
    void CheckUncheckAll(bool isChecked);
    void RemoveSelectedMontages();
    void SaveAnonymizedFile();
    void GenerateLookUpTableTemplate();
    void BrowseForLookUpTable();
    void SaveLUT();
    void ExportFileInformations();
    void CheckFileDuplicate();
    void updateFileCount();
    void showAboutDialog();
    void clearLog();
    void OnNoteItemChanged(QTableWidgetItem* item);
    void OnNotesContextMenu(const QPoint& pos);

private:
    Ui::TRCAnonymizer ui;
    QFileSystemModel* m_localFileSystemModel = nullptr;
    QHash<QString, QString> m_fileMapDictionnary;
    IFile* m_eegFile = nullptr;
    int m_selectedItems = 0;
    bool m_lock = false;
    bool m_notesLock = false;
    bool m_isAlreadyRunning = false;
    Qt::CaseSensitivity m_researchCaseSensitiv = Qt::CaseInsensitive;
    QThread* thread = nullptr;
    AnonymizationWorker* worker = nullptr;
    LutAnonymizationWorker* worker2 = nullptr;
    InformationExtractionWorker* worker3 = nullptr;
    DuplicateCheckWorker* worker4 = nullptr;

    // Status bar labels
    QLabel* m_statusLabel = nullptr;
    QLabel* m_fileCountLabel = nullptr;
};

#endif
