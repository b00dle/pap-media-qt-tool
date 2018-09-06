#include "char_input_dialog.h"

#include <QHBoxLayout>
#include <QRegExp>
#include <QDebug>

namespace Misc {

CharInputDialog::CharInputDialog(QWidget *parent)
    : QDialog(parent)
    , selected_char_(' ')
    , label_(0)
    , edit_(0)
    , ok_(0)
    , cancel_(0)
{
    setWindowTitle(tr("Select Key"));

    initWidgets();
    initLayout();
}

void CharInputDialog::setDialogMessage(const QString& text)
{
    label_->setText(text);
}

const QString CharInputDialog::getDialogMessage() const
{
    return label_->text();
}

const QChar &CharInputDialog::getChar() const
{
    return selected_char_;
}

void CharInputDialog::setChar(const QChar& c)
{
    selected_char_ = c;
    edit_->setText(QString(c));
}

void CharInputDialog::onOk()
{
    if(edit_->text().size() > 0)
        accept();
    else
        reject();
}

void CharInputDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter)
        onOk();

    QRegExp r("[A-Za-z0-9]");
    QString cap((char)e->key());
    if(r.exactMatch(cap))
        setChar(e->key());
}

void CharInputDialog::initWidgets()
{
    label_ = new QLabel(this);
    label_->setText(tr("Press a Letter or Number key to change selection. Selection:"));

    edit_ = new QLineEdit(this);
    edit_->setReadOnly(true);

    ok_ = new QPushButton(tr("OK"), this);

    cancel_ = new QPushButton(tr("Cancel"), this);

    connect(ok_, SIGNAL(pressed()),
            this, SLOT(onOk()));
    connect(cancel_, SIGNAL(pressed()),
            this, SLOT(reject()));
}

void CharInputDialog::initLayout()
{
    QHBoxLayout* upper_layout = new QHBoxLayout;
    upper_layout->addWidget(label_, 1);
    upper_layout->addWidget(edit_, 0);

    QHBoxLayout* lower_layout = new QHBoxLayout;
    lower_layout->addStretch(2);
    lower_layout->addWidget(ok_, 1);
    lower_layout->addWidget(cancel_, 1);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(upper_layout);
    layout->addLayout(lower_layout);

    setLayout(layout);
}

} // namespace Misc
