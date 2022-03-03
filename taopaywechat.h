#ifndef TAOPAYWECHAT_H
#define TAOPAYWECHAT_H

#include <QWidget>

namespace Ui {
class TaoPayWeChat;
}


class TaoPayWeChat : public QWidget
{
    Q_OBJECT

public:
    explicit TaoPayWeChat(QWidget *parent = 0);
    ~TaoPayWeChat();

private slots:
    void on_btnPay_clicked();

    void on_btnShare_clicked();

private:
    Ui::TaoPayWeChat *ui;
};

#endif // TAOPAYWECHAT_H
