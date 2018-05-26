#include "addLogin.h"
#include "ui_addLogin.h"

QString GetRandomString();                  // function prototype for GetRandomString()

addLogin::addLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addLogin)
{
    ui->setupUi(this);
}

addLogin::~addLogin()
{
    delete ui;
}

// Closes current window
// Directs to main menu window
void addLogin::on_pushButton_BACK_clicked()
{
    MainWindow* mainwin = new MainWindow(this);
    this->close();
    mainwin->show();
}

// When submit is clicked
// Takes the user input
// -> generates a random string (salt)
// -> adds the salt to the user inputted password
// -> hashes the password+salt using the SHA256 algorithm (almost-unique 32 byte signature text)
// -> changes it to hex (ASCII for string)
// -> stores the user input (email,username, password(hash), and the random generated string for that user input to be made as a login record in SQLite
// -> checks if it successfully executes or not
void addLogin::on_pushButton_SUBMIT_clicked()
{
    QString email    = ui->lineEdit_EMAIL->text();              // obtains user input for email
    QString username = ui->lineEdit_USERNAME->text();           // obtains user input for username
    QString password = ui->lineEdit_PASSWORD->text();           // obtains user input for password

    QString salt = GetRandomString();                           // calls GetRandomString to generate a random 12 character string

    qDebug() << password << " <---- BEFORE SALT";               // error-check in console for password
    qDebug() << salt     << " <---- AFTER random string";       // error-check in console for random generated string (salt)

    password = password + salt;                                 // combines the password and salt

    qDebug() << password << " <---- AFTER SALT";                // error-check in console for the password+salt

    QByteArray hash = QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Sha256);     // cmputes the hash of the password+salt with the SHA256 algorithm

    hash = hash.toHex();            // converts the hash to hex for ASCII (string)

    qDebug() << hash << " <----- HASH";                         // error check in console for generated hash

    bool n;

    n = loginManager::instance().createLoginAccount(login(email, username, hash, salt));            // executes create login record

    if(n == true)       // checks if its successfull
    {
        QSqlQuery query;
        query.prepare("INSERT INTO loginInfo (email, username, password) VALUES (:email, :username, :password)");       // puts the record into the loginInfo table with its correct variables
        query.bindValue(":email", email);           // gets user input into SQL for email
        query.bindValue(":username", username);     // gets user input into SQL for username
        query.bindValue(":password", password);     // gets user input into SQL for password
        query.exec();                               // executes the prepare

        QMessageBox::information(this,QObject::tr("System Message"),tr("Login record has been created!"),QMessageBox::Ok); // message if success
    }
    else
    {
        QMessageBox::information(this,QObject::tr("System Message"),tr("Login record creation has failed!"),QMessageBox::Ok);  // error message
    }

    ui->lineEdit_EMAIL->clear();            // clears user input - email
    ui->lineEdit_USERNAME->clear();         // clears user input - username
    ui->lineEdit_PASSWORD->clear();         // clears user input - password
}

// Function to generate a random string
QString GetRandomString()
{
   const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");      // up to 64 characters
   const int randomStringLength = 12;       // can change how long the salt can be (make it more complex; harder to be brute forced in)

   QString randomString;
   for(int i=0; i<randomStringLength; ++i)
   {
       int index = qrand() % possibleCharacters.length();
       QChar nextChar = possibleCharacters.at(index);
       randomString.append(nextChar);
   }
   return randomString;
}
