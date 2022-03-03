#ifndef TAOHELPTEMPLATE_H
#define TAOHELPTEMPLATE_H

#include <QMainWindow>
#include <QUrl>
#include <QDesktopServices>
#include <QToolBar>
#include <QToolButton>

#include "taosettings.h"

namespace Ui {
class TaoHelpTemplate;
}

class TaoHelpTemplate : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaoHelpTemplate(QWidget *parent = 0);
    ~TaoHelpTemplate();

    void setHelpContent(QString title, QString strFile);  //, QString strHelp);

    void setToolName(QString name);

    void setHiddenCheckBoxDisplay(bool hidden);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void on_btnClose_clicked();

    void on_btnVisitWeb_clicked();

    void on_chkBoxDisplay_stateChanged(int arg1);

    void homeClicked();

    void backwardClicked();

    void forwardClicked();

private:
    Ui::TaoHelpTemplate *ui;

    QString toolName;

    void createToolBar();

    QToolBar *mainToolBar;
    QToolButton *btnHome;
    QToolButton *btnBackward;
    QToolButton *btnForward;
};

#endif // TAOHELPTEMPLATE_H
