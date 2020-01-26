#include <QEvent>
#include "Connect.h"

const QString login = "guest";
const QString password = "guest";

Connect::Connect( QWidget* parent ):
    QDialog( parent )
{
    applyButton = new QPushButton( this );
    cancelButton = new QPushButton( this );

    addrLabel = new QLabel( this );
    loginLabel = new QLabel( this );
    passLabel = new QLabel( this );

    addressTextBox = new QLineEdit( this );
    loginTextBox = new QLineEdit( this );
    passTextBox = new QLineEdit( this );


    layout = new QGridLayout( this );
    layout->addWidget( addrLabel, 0, 0 );
    layout->addWidget( addressTextBox, 0, 1 );

    layout->addWidget( loginLabel, 1, 0 );
    layout->addWidget( loginTextBox, 1, 1 );

    layout->addWidget( passLabel, 2, 0 );
    layout->addWidget( passTextBox, 2, 1 );

    layout->addWidget( applyButton, 3, 0 );
    layout->addWidget( cancelButton, 3, 1 );

    this->setFixedSize( 280, 150 );

    addrLabel->setText( tr("Host:") );
    loginLabel->setText( tr("Login:") );
    passLabel->setText( tr("Pass:") );

    loginTextBox->setText( login );
    passTextBox->setText( password );

    passTextBox->setEchoMode( QLineEdit::Password );

    applyButton->setText( tr("Apply") );
    cancelButton->setText( tr("Cancel") );

    applyButton->setDefault( true );

    loginTextBox->installEventFilter( this );
    passTextBox->installEventFilter( this );

    connect( applyButton, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
}

Connect::~Connect(){
}

void Connect::setAddressString(const QString& address,quint16 port){
    setAddressString( QHostAddress(address), port );
}

void Connect::setAddressString(const QHostAddress& address, quint16 port){
    if( !addressTextBox ){
        return;}
    addressTextBox->setText(
        QString("%1:%2").arg(address.toString()).arg(port)
    );
}

void Connect::setLogin( const QString& login ){
    loginTextBox->setText( login );
}

QString Connect::getAddress() const{
    return address;
}

QString Connect::getLogin() const{
    return login;
}

QString Connect::getPassword() const{
    return pass;
}

quint16 Connect::getPort(){
    return port;
}

void Connect::accept(){
    QHostAddress addr( addressTextBox->text().replace(QRegExp(":.*"), "") );
    bool ok;
    port = addressTextBox->text().replace( QRegExp(".*:"), "" ).toInt( &ok );
    login = loginTextBox->text();
    pass = passTextBox->text();
    if( !addr.isNull() && ok ){
        address = addr.toString();
        qDebug(
            "Server address and port: %s:%d",
            qPrintable(address),
            port
        );
        done( QDialog::Accepted );
        return;
    }

    QMessageBox::warning(
        this,
        tr("Warning"),
        tr("Specify the valid IPv4 address")
    );

    addressTextBox->setFocus();
}

bool Connect::eventFilter( QObject* object, QEvent* event ){
    if( event->type() == QEvent::MouseButtonPress )
    {
        if( object == loginTextBox )
        {
            loginTextBox->selectAll();
            return true;
        }

        if( object == passTextBox )
        {
            passTextBox->selectAll();
            return true;
        }

        return false;
    }

    return QDialog::eventFilter( object, event );
}
