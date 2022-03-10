/********************************************************************************
** Form generated from reading UI file 'UserInterface.ui'
**
** Created by: Qt User Interface Compiler version 6.2.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERINTERFACE_H
#define UI_USERINTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserInterface
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *UserInterface)
    {
        if (UserInterface->objectName().isEmpty())
            UserInterface->setObjectName(QString::fromUtf8("UserInterface"));
        UserInterface->resize(800, 600);
        centralwidget = new QWidget(UserInterface);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        UserInterface->setCentralWidget(centralwidget);
        menubar = new QMenuBar(UserInterface);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        UserInterface->setMenuBar(menubar);
        statusbar = new QStatusBar(UserInterface);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        UserInterface->setStatusBar(statusbar);

        retranslateUi(UserInterface);

        QMetaObject::connectSlotsByName(UserInterface);
    } // setupUi

    void retranslateUi(QMainWindow *UserInterface)
    {
        UserInterface->setWindowTitle(QCoreApplication::translate("UserInterface", "UserInterface", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserInterface: public Ui_UserInterface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERINTERFACE_H
