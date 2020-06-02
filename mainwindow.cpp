#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

void FileScopeFunction(int test)
{
    qDebug("File Scope: Got a print action call with the value %d", test);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTableWidgetItem* itemTest = new QTableWidgetItem();
    // lifetime?
    QComboBox* itemComboBox = new QComboBox();
    itemComboBox->addItem("Zero");
    itemComboBox->addItem("One");
    itemComboBox->addItem("Two");

    // Non-Qt function pointers (an overview)
    {
        // C function pointer
        typedef void (*FileScopeFunction_t)(int);

        // C++ method pointer
        typedef void (MainWindow::*MainWindowClassFunction_t)(int);

        FileScopeFunction_t test = &FileScopeFunction;
        test(85);
        MainWindowClassFunction_t test2 = &MainWindow::OnCellComboBoxChanged_Print;
        (this->*test2)(86);

        // now without typedef,
        void (MainWindow::*alsoMyClassFunc)(int) = &MainWindow::OnCellComboBoxChanged_Print;
        (this->*alsoMyClassFunc)(87);
    }

    // Qt FunctionPointer<>
    {
        // interestingly, this causes an infinite recursion
        //QtPrivate::FunctionPointer<void (MainWindow::*)(const QString &)>::Object testing;

        void (MainWindow::*methodFunction)(const QString &);

        // wow
        // I love C++ function pointer syntax.
        QtPrivate::FunctionPointer<void (QComboBox::*)(int)> comboBoxFunction;

        // no, confusingly a FunctionPointer isn't assignable to a function pointer.
        //  comboBoxFunction = &QComboBox::setCurrentIndex;

        // Note that Object would be ComboBox, not ComboBox*
        QtPrivate::FunctionPointer<void (QComboBox::*)(const QString &)>::Object* comboBoxAlso;


        comboBoxAlso = itemComboBox;

        void (QComboBox::*comboBox_IndexChanged)(int) = &QComboBox::currentIndexChanged;

        QObject::connect(itemComboBox, comboBox_IndexChanged, this, &MainWindow::OnCellComboBoxChanged, Qt::AutoConnection);

        // why doesn't this work?
        //QObject::connect(itemComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::OnCellComboBoxChanged, Qt::AutoConnection);
    }

    ui->tblTest->setRowCount(2);

    ui->tblTest->setItem(0, 0, itemTest);
    ui->tblTest->setCellWidget(0, 0, itemComboBox);
}

void MainWindow::OnCellComboBoxChanged(int index)
{
    QMessageBox::information(this, tr("Event received"), tr("Combo box index changed, new index: %1").arg(index));
}

void MainWindow::OnCellComboBoxChanged_Print(int index)
{
    qDebug("Class Scope: Got a print action call with the value %d", index);
}

MainWindow::~MainWindow()
{
    delete ui;
}
