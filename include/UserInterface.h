#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class UserInterface; }
QT_END_NAMESPACE

class UserInterface : public QMainWindow
{
    Q_OBJECT

public:
    UserInterface(QWidget *parent = nullptr);
    ~UserInterface();

private:
    Ui::UserInterface *ui;
};
#endif // USERINTERFACE_H
