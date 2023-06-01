#ifndef TRCANONYMIZER_H
#define TRCANONYMIZER_H

#include "ui_TRCAnonymizer.h"
#include <QtWidgets/QMainWindow>
#include <QFileSystemModel>
#include <QHash>
#include "MicromedFile.h"
#include "AnonymizationWorker.h"
#include <QThread>
//#include <QtWidgets>

class TRCAnonymizer : public QMainWindow
{
	Q_OBJECT

public:
    TRCAnonymizer(QWidget *parent = nullptr);
	~TRCAnonymizer();

private:
    void LoadFolder();
    void LoadTreeViewUI(QString initialFolder);
    void LoadMontagesUI(std::vector<montagesOfTrace> montages);

private slots:
    void AddFilesToList();
    void RemoveFilesFromList();
    void OnItemSelected(QListWidgetItem* item);
    void OnItemChanged(QListWidgetItem* item);
    void OnCurrentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
    void ToggleEditableFields();
    void AnonymizeHeader();
    void CheckUncheckAll(bool isChecked);
    void RemoveSelectedMontages();
    void SaveAnonymizedFile();

private:
    Ui::TRCAnonymizer ui;
    QFileSystemModel* m_localFileSystemModel = nullptr;
    QHash<QString, QString> m_fileMapDictionnary;
    MicromedFile m_micromedFile;
    int m_selectedItems = 0;
    bool m_lock = false;
    bool m_isAlreadyRunning = false;
    QThread* thread = nullptr;
    AnonymizationWorker* worker = nullptr;
};

#endif
