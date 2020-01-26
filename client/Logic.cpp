#include <QAbstractSocket>
#include "Logic.h"

const QString& config_File = "config.ini";
const quint16 server_Port = 1234;
const quint16 timeout = 5000;

Logic::Logic( Game* game ):
    game(game),
    serverAddress( QHostAddress::LocalHost ),
    serverPort(server_Port)
{
    client = new QTcpSocket( this );
    connect(
        client, SIGNAL(readyRead()),
        this, SLOT(onDataReceived())
    );

    connect(
        client, SIGNAL(connected()),
        this, SLOT(onConnected())
    );

    connect(
        client, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(onError(QAbstractSocket::SocketError))
    );

    readConfig();
}

Logic::~Logic(){
    saveConfig();
}

void Logic::readConfig(){
    if( !QFile::exists(config_File) ) {
        qDebug() << "Config file does not exists!";
        return;
    }

    QFile cf( config_File );

    if( !cf.open(QFile::ReadOnly) )
        return;

    QByteArray line;
    QRegExp rx( "(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+):(\\w+):(.+):" );

    while( !cf.atEnd() )
    {
        line = cf.readLine();

        if( rx.indexIn(line) == -1 )
            continue;

        qDebug() << "Setting connection info";
        setConnectionInfo(
            rx.cap(1),
            rx.cap(2).toUInt(),
            rx.cap(3),
            rx.cap(4)
        );
        break;
    }

    cf.close();
}

void Logic::saveConfig(){
    QFile cf( config_File );

    if( !cf.open(QIODevice::WriteOnly | QIODevice::Text) )
        return;

    cf.write(
        qPrintable(
            QString("%1:%2:%3:%4:\n")
            .arg(serverAddress.toString())
            .arg(serverPort)
            .arg(game->getLogin())
            .arg(game->getPassword())
        )
    );
    cf.close();
}

void Logic::onMousePressed( const QPoint& pos, bool setShip ){
    if( game->getState() == ST_PLACING_SHIPS ) {
        QPoint point = field.getMyFieldCoord( pos );

        if( point.x() == -1 || point.y() == -1 ){
            return;}
        qDebug() << "Ship at" << point.x() << point.y();
        game->setMyCell( point.x(), point.y(), setShip ? SHIP : CLEAR );
        emit stateChanged();
        return;
    }

    if( game->getState() == ST_MAKING_STEP ){
        QPoint point = field.getEnemyFieldCoord( pos );
        if( point.x() == -1 || point.y() == -1 )
            return;
        qDebug() << "Going to" << point.x() << point.y();
        Cell cell = game->getEnemyCell( point.x(), point.y() );
        if( cell != CLEAR )
            return;
        game->setEnemyCell( point.x(), point.y(), DOT );
        QString cmd;
        cmd = QString( "step:%1:%2:" ).arg( point.x() ).arg( point.y() );
        qDebug() << cmd;
        client->write( cmd.toLocal8Bit() );
        game->setState( ST_WAITING_STEP );
        emit stateChanged();
        return;
    }
}

void Logic::onDataReceived(){
    QString data;
    data = client->readAll();
    qDebug() << "Data:" << data;
    parseData( data );
    emit stateChanged();
}

void Logic::parseData( const QString& data ){
    parseFound( data );
    parseWrongField( data );
    parseWrongStep( data );
    parseFields( data );
    parseGo( data );
    parseGameResult( data );
    parseErrorInfo( data );
    parsePing( data );
}

bool Logic::parsePing( const QString& data ){
    QRegExp rx( "ping:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Ping request";
    client->write("pong:\n");
    return true;
}

bool Logic::parseFound( const QString& data ){
    QRegExp rx( "found:((\\w|\\d)+):" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Found opponent:" << rx.cap( 1 );
    emit gameOpponent( rx.cap(1) );
    return true;
}

bool Logic::parseGo( const QString& data ){
    QRegExp rx( "go:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Now making step!";
    game->setState( ST_MAKING_STEP );
    return true;
}

bool Logic::parseErrorInfo( const QString& data ){
    QRegExp rx( "wronguser:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Wrong user";
    game->setState( ST_PLACING_SHIPS );
    connectionError = true;
    emit gameError( GEM_WRONG_USER );
    return true;
}

bool Logic::parseWrongStep( const QString& data ){
    QRegExp rx( "wrongstep:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Maked wrong step";
    game->setState( ST_MAKING_STEP );
    return true;
}

bool Logic::parseWrongField( const QString& data ){
    QRegExp rx( "wrongfield:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Maked wrong field";
    game->setState( ST_PLACING_SHIPS );
    return true;
}

bool Logic::parseFields( const QString& data ){
    QRegExp rx( "field(\\d):(\\w+):(\\d):(\\d):" );

    int pos = 0;
    while( (pos = rx.indexIn(data, pos)) != -1 )
    {
        const QString& type = rx.cap( 2 );
        int field = rx.cap( 1 ).toInt();
        int xpos = rx.cap( 3 ).toInt();
        int ypos = rx.cap( 4 ).toInt();

        Cell cell = type == "half" ? HALF: type == "kill"? SHIP: DOT;
        markShip( xpos, ypos, cell, field == 2 );
        pos += rx.matchedLength();
    }
    return pos;
}
void Logic::markShip( int x, int y, Cell cell, bool atEnemyField ){
    if( cell == SHIP )
    {
        if( !atEnemyField )
            cell = HALF;
        else
            for( int i = -1; i <= 1; i++ )
            {
                markEnemyPoint( x + 1, y + i, DOT );
                markEnemyPoint( x - 1, y + i, DOT );
                markEnemyPoint( x + i, y + 1, DOT );
                markEnemyPoint( x + i, y - 1, DOT );
            }
    }

    if( atEnemyField )
        game->setEnemyCell( x, y, cell );
    else
        game->setMyCell( x, y, cell );
}

void Logic::markEnemyPoint( int x, int y, Cell cell )
{
    if( game->getEnemyCell(x, y) == CLEAR )
        game->setEnemyCell( x, y, cell );
}

bool Logic::parseGameResult( const QString& data )
{
    QRegExp rx( "win:" );

    if( rx.indexIn(data) != -1 )
    {
        qDebug() << "We win!";
        emit gameResult( GR_WON );
        game->setState( ST_PLACING_SHIPS );
        game->clearMyField();
        game->clearEnemyField();
        return true;
    }

    QRegExp rx2( "lose:" );

    if( rx2.indexIn(data) != -1 )
    {
        qDebug() << "We lose!";
        emit gameResult( GR_LOST );
        game->setState( ST_PLACING_SHIPS );
        game->clearMyField();
        game->clearEnemyField();
        return true;
    }

    return false;
}

void Logic::onGameStart()
{
    if( !game->checkMyField() )
    {
        emit gameError( GEM_WRONG_FIELD );
        return;
    }

    if( client->state() == QAbstractSocket::ConnectedState )
    {
        emit gameError( GEM_ALREADY_CONNECTED );
        return;
    }

    client->connectToHost( serverAddress, serverPort );

    if( !client->waitForConnected(timeout) )
    {
        emit gameError( GEM_SERVER_UNAVAILABLE );
        return;
    }

    qDebug(
        "Connected to host %s:%d as %s",
        qPrintable(serverAddress.toString()),
        serverPort,
        qPrintable(game->getLogin())
    );
}

void Logic::onGameQuit()
{
    if( client->state() == QAbstractSocket::ConnectedState )
    {
        qDebug() << "Disconnecting from host";
        client->write( "disconnect:" );
        client->disconnectFromHost();
        game->clearEnemyField();
        game->clearMyField();
        game->setState( ST_PLACING_SHIPS );
    }
}

void Logic::clearFields()
{
    if( game->getState() != ST_PLACING_SHIPS )
        return;

    game->clearEnemyField();
    game->clearMyField();
}

void Logic::randomField()
{
    if( game->getState() != ST_PLACING_SHIPS )
        return;

    game->clearMyField();

    for( int i = 1, k = 4; i <= 4; i++, k-- )
        for( int j = 0; j < i; j++ )
            placeShipAtRandom( k );
}

void Logic::placeShipAtRandom( int size )
{
    int p;
    int q;
    bool r;
    bool isOk = true;

    while( isOk )
    {
        p = qrand() % ( 10 - size + 1 );
        q = qrand() % ( 10 - size + 1 );
        r = qrand() % 2;

        for(
            int k = r * p + !r * q - 1;
            k < (r * p + !r * q + size + 1);
            k++
        )
            if(
                game->getMyCell(
                    r * k + !r * p,
                    r * q + !r * k
                ) == SHIP ||
                game->getMyCell(
                    r * k + !r * (p - 1),
                    r * (q - 1) + !r * k
                ) == SHIP ||
                game->getMyCell(
                    r * k + !r * (p + 1),
                    r * (q + 1) + !r * k
                ) == SHIP
            )
                isOk = false;

        isOk = ! isOk;
    }

    for(
        int k = r * p + !r * q;
        k < (r * p + !r * q + size);
        k++
    )
        game->setMyCell(
            r * k + !r * p,
            r * q + !r * k,
            SHIP
        );
}

void Logic::onError( QAbstractSocket::SocketError socketError )
{
    qDebug() << client->errorString();

    if(
        game->getState() == ST_WAITING_STEP ||
        game->getState() == ST_MAKING_STEP
    )
        game->setState( ST_PLACING_SHIPS );

    if( socketError == QAbstractSocket::ConnectionRefusedError )
        emit gameError( GEM_SERVER_CONNECTION_REFUSED );
}

void Logic::onConnected()
{
    QString response;
    QString request;
    connectionError = false;

    request = QString( "mbclient:2:%1:%2:" )
        .arg( game->getLogin() )
        .arg( game->getPassword() );

    client->write( request.toLocal8Bit() );

    if( !client->waitForReadyRead(timeout) )
        return;

    if( connectionError )
        return;

    response = client->readAll();
    qDebug() << response;

    request = "field:4:" + game->getMyField() + ":";
    client->write( request.toLocal8Bit() );

    qDebug() << request;
    game->setState( ST_WAITING_STEP );
}

State Logic::getState() const
{
    return game->getState();
}

void Logic::setConnectionInfo(
    const QString& address,
    quint16 port,
    const QString& login,
    const QString& password
)
{
    serverAddress = QHostAddress( address );
    serverPort = port;
    game->setLogin( login );
    game->setPassword( password );
}

QString Logic::getServerAddress() const
{
    return serverAddress.toString();
}

quint16 Logic::getServerPort() const
{
    return serverPort;
}

QString Logic::getUserLogin() const
{
    return game->getLogin();
}
