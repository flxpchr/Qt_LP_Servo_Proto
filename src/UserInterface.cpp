#include "UserInterface.h"
#include "ui_UserInterface.h"

UserInterface::UserInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserInterface)
{
    ui->setupUi(this);
}

UserInterface::~UserInterface()
{
    delete ui;
}

