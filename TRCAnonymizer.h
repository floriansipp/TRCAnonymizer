#ifndef TRCANONYMIZER_H
#define TRCANONYMIZER_H

#include "ui_TRCAnonymizer.h"
#include <QtWidgets/QMainWindow>
#include <QFileSystemModel>
#include <QHash>
#include "AnonymizationWorker.h"
#include "LutAnonymizationWorker.h"
#include <QThread>
#include "IFile.h"

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
    QHash<std::string, std::string> LoadLUT(std::string path);
    void EnableFieldsEdit(bool editable);
    IFile* LoadEegFile(QString filepath);

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
    void SaveLUT();

private:
    Ui::TRCAnonymizer ui;
    QFileSystemModel* m_localFileSystemModel = nullptr;
    QHash<QString, QString> m_fileMapDictionnary;
    IFile* m_eegFile = nullptr;
    int m_selectedItems = 0;
    bool m_lock = false;
    bool m_isAlreadyRunning = false;
    Qt::CaseSensitivity m_researchCaseSensitiv = Qt::CaseInsensitive;
    QThread* thread = nullptr;
    AnonymizationWorker* worker = nullptr;
    LutAnonymizationWorker* worker2 = nullptr;
};

#endif
