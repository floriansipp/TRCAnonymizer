#ifndef TRCANONYMIZER_H
#define TRCANONYMIZER_H

#include "ui_TRCAnonymizer.h"
#include <QtWidgets/QMainWindow>
#include <QFileSystemModel>
#include <QHash>
#include "MicromedFile.h"

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
};

#endif
