#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    qsrand( QTime::currentTime().msec() );

    ui->setupUi( this );
    pictures = new Textures;
    pictures->load();

    ui->labelStatus->setStyleSheet( "QLabel { color : #00157f; }" );
    ui->labelOpponent->setStyleSheet( "QLabel { color : #00157f; }" );
    ui->labelOpponent->clear();
    game = new Game;
    logic = new Logic(game);

    connect( logic, SIGNAL(stateChanged()), this, SLOT(redraw()) );
    connect(
        logic,
        SIGNAL(gameResult(GameResult)),
        this,
        SLOT(showGameResult(GameResult))
    );
    connect(
        logic,
        SIGNAL(gameError(GameErrorMessage)),
        this,
        SLOT(showGameError(GameErrorMessage))
    );
    connect(
        logic,
        SIGNAL(gameOpponent(QString)),
        this,
        SLOT(changeGameOpponent(QString))
    );

    this->redraw();
}

MainWindow::~MainWindow()
{
    delete logic;
    delete game;
    delete pictures;
    delete ui;
}

void MainWindow::setStatus( const QString& status )
{
    ui->labelStatus->setText( tr("Status: ") + status );
}

void MainWindow::changeGameOpponent( const QString& name )
{
    ui->labelOpponent->setText( tr("Opponent: ") + name );
}

void MainWindow::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );

    const int deltaY = this->centralWidget()->y();

    QPainter painter( this );
    painter.drawImage(
        0,
        deltaY,
        pictures->get("field")
    );

    painter.drawImage( field.leftFieldX, field.leftFieldY + deltaY, myFieldImage() );
    painter.drawImage( field.rightFieldX, field.rightFieldY + deltaY, enemyFieldImage() );

    switch( logic->getState() )
    {
    case ST_PLACING_SHIPS:
        setStatus( "placing ships" );
        break;

    case ST_MAKING_STEP:
        setStatus( "your step" );
        break;

    case ST_WAITING_STEP:
        setStatus( "wait for enemy" );
        break;
    }
}

QImage MainWindow::myFieldImage()
{
    return getFieldImage( 0 );
}

QImage MainWindow::enemyFieldImage()
{
    return getFieldImage( 1 );
}

QImage MainWindow::getFieldImage( char fld )
{
    QImage image( field.fieldWidth, field.fieldHeight, QImage::Format_ARGB32 );
    Cell cell;
    image.fill( 0 );
    QPainter painter( &image );

    double cfx = 1.0 * field.fieldWidth / 10.0;
    double cfy = 1.0 * field.fieldHeight / 10.0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
        {
            if( fld == 0 )
                cell = game->getMyCell( i, j );
            else
                cell = game->getEnemyCell( i, j );

            switch( cell )
            {
            case DOT:
                painter.drawImage( i * cfx, j * cfy, pictures->get("dot") );
                break;

            case HALF:
                painter.drawImage(
                    i * cfx,
                    j * cfy,
                    fld ? pictures->get("half") : pictures->get("redhalf")
                );
                break;

            case SHIP:
                painter.drawImage( i * cfx, j * cfy, pictures->get("full") );
                break;

            default:
                break;
            }
        }

    return image;
}

void MainWindow::mousePressEvent( QMouseEvent* ev )
{
    QPoint pos = ev->pos();
    pos.setY( pos.y() - this->centralWidget()->y() );
    logic->onMousePressed( pos, ev->button() == Qt::LeftButton );
}

void MainWindow::closeEvent( QCloseEvent* event )
{
    logic->onGameQuit();
    event->accept();
}

void MainWindow::on_connectButton_clicked()
{
    Connect* connectionDialog = new Connect( this );

    connectionDialog->setModal( true );
    connectionDialog->setAddressString(
        logic->getServerAddress(),
        logic->getServerPort()
    );
    connectionDialog->setLogin( logic->getUserLogin() );

    if( connectionDialog->exec() != QDialog::Accepted )
        return;

    logic->setConnectionInfo(
        connectionDialog->getAddress(),
        connectionDialog->getPort(),
        connectionDialog->getLogin(),
        connectionDialog->getPassword()
    );
    logic->onGameStart();
    redraw();
}

void MainWindow::redraw()
{
    if( logic->getState() == ST_PLACING_SHIPS )
        ui->labelOpponent->clear();

    if( logic->getState() == ST_PLACING_SHIPS )
    {
        ui->actionStart->setDisabled(false);
        ui->actionLeave->setDisabled(true);
    }
    else
    {
        ui->actionStart->setDisabled(true);
        ui->actionLeave->setDisabled(false);
    }

    this->update();
}

void MainWindow::on_exitButton_clicked()
{
    close();
}

void MainWindow::on_clearButton_clicked()
{
    logic->clearFields();
    this->update();
}

void MainWindow::showGameResult( GameResult result )
{
    if( result == GR_NONE )
        return;

    QString messageString = result == GR_WON
        ? tr( "You win!" )
        : tr( "You lose!" );

    this->update();
    QMessageBox::information( this, tr("Game result"), messageString );
}

void MainWindow::showGameError( GameErrorMessage message )
{
    QString messageString;

    switch( message )
    {
    case GEM_WRONG_FIELD:
        messageString = tr( "Wrong ships placement!" );
        break;

    case GEM_WRONG_USER:
        messageString = tr( "Wrong user passed!" );
        break;

    case GEM_ALREADY_CONNECTED:
        messageString = tr( "You are already connected!" );
        break;

    case GEM_SERVER_ERROR:
        messageString = tr( "Server error!" );
        break;

    case GEM_SERVER_CONNECTION_REFUSED:
    case GEM_SERVER_UNAVAILABLE:
        messageString = tr(
            "Cannot connect to the server."
        );
        break;

    default:
        return;
    }

    this->update();
    QMessageBox::information( this, tr("Game Info"), messageString );
}

void MainWindow::on_randomButton_clicked()
{
    logic->randomField();
    this->update();
}

void MainWindow::on_actionLeave_activated()
{
    logic->onGameQuit();
    QMessageBox::information(
        this,
        tr("Game Info"),
        tr("You have disconnected!")
    );

    redraw();
}








