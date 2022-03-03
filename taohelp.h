

#ifndef TAOHELP_H
#define TAOHELP_H

#include <QWidget>

#include <QDesktopServices>
#include <QUrl>

#include "taosettings.h"

namespace Ui {
class TaoHelp;
}

class TaoHelp : public QWidget
{
    Q_OBJECT

public:
    explicit TaoHelp(QWidget *parent = 0);
    ~TaoHelp();

private slots:
    void on_btnGo2WebSite_clicked();

    void on_chkBoxStartShowHelp_stateChanged(int curState);

    void on_btnClose_clicked();

private:
    Ui::TaoHelp *ui;
};

#endif // TAOHELP_H
