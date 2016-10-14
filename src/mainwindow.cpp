#include "mainwindow.h"
#include "ui_mainwindow.h"

const QString MainWindow::MAIN_WINDOW_TITLE_STRING{"QPassword"};
const QString MainWindow::MAIN_WINDOW_ICON_PATH{":/icons/qpassword.png"};
const QString MainWindow::NON_MATCHING_PASSWORD_STRING{"The passwords you entered do not match!"};
const QString MainWindow::PASSWORD_PROMPT_STRING{"Please enter a password"};
const QString MainWindow::CURRENT_PASSWORD_INCORRECT_STRING{"The password you entered does not match stored password"};
const QString MainWindow::INITIAL_PASSWORD_STORED_STRING{"You password was successfully set"};
const QString MainWindow::PASSWORD_CHANGE_SUCCESSFUL_STRING{"Your password was successfully changed"};
const QString MainWindow::PASSWORD_CLEARED_STRING{"Your password was successfully cleared"};
const QString MainWindow::PASSWORD_TOO_SHORT_STRING{"Your password must be at least 3 characters in length"};
const QString MainWindow::PASSWORD_DOES_NOT_CONTAIN_UPPER_CASE_STRING{"Your password must contain at least one upper case letter"};
const QString MainWindow::PYTHON_NOT_INSTALLED_STRING{"QPassword requires python to be installed and in the system PATH variable to run, exiting program"};
const int MainWindow::PASSWORD_CHANGE_SUCCESSFUL_STRING_TIMEOUT{2500};
const int MainWindow::INITIAL_PASSWORD_STORED_TIMEOUT{2500};
const int MainWindow::PASSWORD_CLEARED_STRING_TIMEOUT{1000};
const int MainWindow::PASSWORD_LENGTH_MINIMUM{3};
const int MainWindow::SALT_DIGIT_COUNT{32};
const int MainWindow::ASCII_LOW_LIMIT{32};
const int MainWindow::ASCII_HIGH_LIMIT{126};
const std::vector<int> MainWindow::asciiControlCharacters { (int)'\\',
                                                            (int)'\'',
                                                            (int)'\"',
                                                            (int)'`' };

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ptPasswordSaved{false},
    m_ptPassword{""},
    m_hPasswordSaved{false},
    m_hHashedPassword{""},
    m_hsPasswordSaved{false},
    m_hsSalt{""},
    m_hsHashedAndSaltedPassword{""},
    m_pythonCrypto{std::make_unique<PythonCrypto>()},
    m_ui(std::make_unique<Ui::MainWindow>())
{
    if (!this->m_pythonCrypto->pythonInstalled()) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(PYTHON_NOT_INSTALLED_STRING);
        errorBox->setWindowIcon(QIcon(MAIN_WINDOW_ICON_PATH));
        errorBox->exec();
        exit(EXIT_FAILURE);
    }

    srand(time(0));
    this->m_ui->setupUi(this);
    this->setWindowTitle(MAIN_WINDOW_TITLE_STRING);
    connect(this->m_ui->ptSubmitButton, SIGNAL(clicked(bool)), this, SLOT(onPtSubmitButtonClicked()));
    connect(this->m_ui->ptClearButton, SIGNAL(clicked(bool)), this, SLOT(onPtClearButtonClicked()));

    this->m_ui->ptCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->tabWidget->setTabOrder(this->m_ui->ptCurrentPasswordLineEdit, this->m_ui->ptPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->ptPasswordLineEdit, this->m_ui->ptSecondPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->ptSecondPasswordLineEdit, this->m_ui->ptSubmitButton);

    connect(this->m_ui->hSubmitButton, SIGNAL(clicked(bool)), this, SLOT(onHSubmitButtonClicked()));
    connect(this->m_ui->hClearButton, SIGNAL(clicked(bool)), this, SLOT(onHClearButtonClicked()));

    this->m_ui->hCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->tabWidget->setTabOrder(this->m_ui->hCurrentPasswordLineEdit, this->m_ui->hPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->hPasswordLineEdit, this->m_ui->hSecondPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->hSecondPasswordLineEdit, this->m_ui->hSubmitButton);

    connect(this->m_ui->hsSubmitButton, SIGNAL(clicked(bool)), this, SLOT(onHsSubmitButtonClicked()));
    connect(this->m_ui->hsClearButton, SIGNAL(clicked(bool)), this, SLOT(onHsClearButtonClicked()));

    this->m_ui->hsCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->tabWidget->setTabOrder(this->m_ui->hsCurrentPasswordLineEdit, this->m_ui->hsPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->hsPasswordLineEdit, this->m_ui->hsSecondPasswordLineEdit);
    this->m_ui->tabWidget->setTabOrder(this->m_ui->hsSecondPasswordLineEdit, this->m_ui->hsSubmitButton);

    connect(this->m_ui->statusBar, SIGNAL(messageChanged(QString)), this, SLOT(onMessageChanged(QString)));
    this->m_ui->statusBar->showMessage(PASSWORD_PROMPT_STRING);
    this->m_ui->ptPasswordLineEdit->setFocus();
}


void MainWindow::onPtSubmitButtonClicked()
{
    if (this->m_ptPasswordSaved) {
        QString maybeCurrentPassword{this->m_ui->ptCurrentPasswordLineEdit->text()};
        if (maybeCurrentPassword == this->m_ui->ptStoredPasswordLineEdit->text()) {
            this->m_ui->ptCurrentPasswordLineEdit->clear();
            ptAttemptPasswordChange();
        } else {
            std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
            errorBox->setText(CURRENT_PASSWORD_INCORRECT_STRING);
            errorBox->exec();
            this->m_ui->ptCurrentPasswordLineEdit->clear();
            this->m_ui->ptPasswordLineEdit->clear();
            this->m_ui->ptSecondPasswordLineEdit->clear();
            this->m_ui->ptCurrentPasswordLineEdit->setFocus();
        }
    } else {
        ptAttemptPasswordChange();
    }
}

QString MainWindow::escapeAllControlCharacters(const QString &stringToEscape)
{
    //TODO: Implement
    return stringToEscape;
}

void MainWindow::ptAttemptPasswordChange()
{
    QString firstPassword {escapeAllControlCharacters(this->m_ui->ptPasswordLineEdit->text())};
    QString secondPassword {escapeAllControlCharacters(this->m_ui->ptSecondPasswordLineEdit->text())};

    if (firstPassword != secondPassword) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(NON_MATCHING_PASSWORD_STRING);
        errorBox->exec();
        this->m_ui->ptPasswordLineEdit->clear();
        this->m_ui->ptSecondPasswordLineEdit->clear();
        this->m_ui->ptPasswordLineEdit->setFocus();
    } else if (firstPassword.length() < PASSWORD_LENGTH_MINIMUM) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(PASSWORD_TOO_SHORT_STRING);
        errorBox->exec();
        this->m_ui->ptPasswordLineEdit->clear();
        this->m_ui->ptSecondPasswordLineEdit->clear();
        this->m_ui->ptPasswordLineEdit->setFocus();
    } /*else if (!containsUpperCaseCharacter(firstPassword)) {
        std::unique_ptr<QMessageBox> errorBox{new QMessageBox(this)};
        errorBox->setText(PASSWORD_DOES_NOT_CONTAIN_UPPER_CASE_STRING);
        errorBox->exec();
        ui->ptPasswordLineEdit->clear();
        ui->ptSecondPasswordLineEdit->clear();
    }*/ else {
        this->m_ui->ptPasswordLineEdit->clear();
        this->m_ui->ptSecondPasswordLineEdit->clear();

        if (this->m_ptPasswordSaved) {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(PASSWORD_CHANGE_SUCCESSFUL_STRING);
            successBox->exec();
        } else {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(INITIAL_PASSWORD_STORED_STRING);
            successBox->exec();
        }

        this->m_ptPasswordSaved = true;
        this->m_ptPassword = firstPassword;
        this->m_ui->ptStoredPasswordLineEdit->setText(this->m_ptPassword);
        this->m_ui->ptCurrentPasswordLineEdit->setEnabled(true);
        this->m_ui->ptCurrentPasswordLineEdit->setFocus();

    }
}

void MainWindow::onPtClearButtonClicked()
{
    this->m_ui->ptCurrentPasswordLineEdit->clear();
    this->m_ui->ptCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->ptStoredPasswordLineEdit->clear();
    this->m_ui->ptPasswordLineEdit->clear();
    this->m_ui->ptSecondPasswordLineEdit->clear();
    this->m_ui->ptPasswordLineEdit->setFocus();

    this->m_ptPasswordSaved = false;
    this->m_ptPassword = "";
    this->m_ui->statusBar->showMessage(PASSWORD_CLEARED_STRING, PASSWORD_CLEARED_STRING_TIMEOUT);
}


void MainWindow::onHSubmitButtonClicked()
{

    if (this->m_hPasswordSaved) {
        QString maybeCurrentPassword {toQString(this->m_pythonCrypto->getSHA256Hash(this->m_ui->hCurrentPasswordLineEdit->text().toStdString()))};
        if (maybeCurrentPassword == this->m_hHashedPassword) {
            this->m_ui->hCurrentPasswordLineEdit->clear();
            hAttemptPasswordChange();
        } else {
            std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
            errorBox->setText(CURRENT_PASSWORD_INCORRECT_STRING);
            errorBox->exec();
            this->m_ui->hCurrentPasswordLineEdit->clear();
            this->m_ui->hPasswordLineEdit->clear();
            this->m_ui->hSecondPasswordLineEdit->clear();
            this->m_ui->hCurrentPasswordLineEdit->setFocus();
        }
    } else {
        hAttemptPasswordChange();
    }

}

void MainWindow::hAttemptPasswordChange()
{
    QString firstPassword{escapeAllControlCharacters(this->m_ui->hPasswordLineEdit->text())};
    QString secondPassword{escapeAllControlCharacters(this->m_ui->hSecondPasswordLineEdit->text())};

    if (firstPassword != secondPassword) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(NON_MATCHING_PASSWORD_STRING);
        errorBox->exec();
        this->m_ui->hPasswordLineEdit->clear();
        this->m_ui->hSecondPasswordLineEdit->clear();
        this->m_ui->hPasswordLineEdit->setFocus();
    } else if (firstPassword.length() < PASSWORD_LENGTH_MINIMUM) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(PASSWORD_TOO_SHORT_STRING);
        errorBox->exec();
        this->m_ui->hPasswordLineEdit->clear();
        this->m_ui->hSecondPasswordLineEdit->clear();
        this->m_ui->hPasswordLineEdit->setFocus();
    } /*else if (!containsUpperCaseCharacter(firstPassword)) {
        std::unique_ptr<QMessageBox> errorBox{new QMessageBox(this)};
        errorBox->setText(PASSWORD_DOES_NOT_CONTAIN_UPPER_CASE_STRING);
        errorBox->exec();
        ui->hPasswordLineEdit->clear();
        ui->hSecondPasswordLineEdit->clear();
    }*/ else {
        this->m_ui->hPasswordLineEdit->clear();
        this->m_ui->hSecondPasswordLineEdit->clear();

        this->m_hHashedPassword = "";
        this->m_ui->hGeneratedHashLineEdit->clear();

        if (this->m_hPasswordSaved) {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(PASSWORD_CHANGE_SUCCESSFUL_STRING);
            successBox->exec();
        } else {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(INITIAL_PASSWORD_STORED_STRING);
            successBox->exec();
        }

        this->m_hPasswordSaved = true;
        this->m_hHashedPassword = toQString(this->m_pythonCrypto->getSHA256Hash(firstPassword.toStdString()));

        this->m_ui->hGeneratedHashLineEdit->setText(this->m_hHashedPassword);
        this->m_ui->hCurrentPasswordLineEdit->setEnabled(true);
        this->m_ui->hCurrentPasswordLineEdit->setFocus();

        this->m_ui->statusBar->showMessage(PASSWORD_CHANGE_SUCCESSFUL_STRING, PASSWORD_CHANGE_SUCCESSFUL_STRING_TIMEOUT);
    }
}

void MainWindow::onHClearButtonClicked()
{
    this->m_ui->hCurrentPasswordLineEdit->clear();
    this->m_ui->hCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->hPasswordLineEdit->clear();
    this->m_ui->hSecondPasswordLineEdit->clear();
    this->m_ui->hGeneratedHashLineEdit->clear();

    this->m_ui->hPasswordLineEdit->setFocus();

    this->m_hPasswordSaved = false;
    this->m_hHashedPassword = "";
    this->m_ui->statusBar->showMessage(PASSWORD_CLEARED_STRING, PASSWORD_CLEARED_STRING_TIMEOUT);
}

void MainWindow::onHsSubmitButtonClicked()
{
    if (this->m_hsPasswordSaved) {
        QString maybeCurrentPasswordBase{this->m_ui->hsCurrentPasswordLineEdit->text()};
        QString maybeCurrentPassword{toQString(this->m_pythonCrypto->getSHA256Hash(this->m_hsSalt.toStdString() + maybeCurrentPasswordBase.toStdString()))};
        if (maybeCurrentPassword == this->m_hsHashedAndSaltedPassword) {
            this->m_ui->hsCurrentPasswordLineEdit->clear();
            hsAttemptPasswordChange();
        } else {
            std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
            errorBox->setText(CURRENT_PASSWORD_INCORRECT_STRING);
            errorBox->exec();
            this->m_ui->hsCurrentPasswordLineEdit->clear();
            this->m_ui->hsPasswordLineEdit->clear();
            this->m_ui->hsSecondPasswordLineEdit->clear();
            this->m_ui->hsPasswordLineEdit->setFocus();
        }
    } else {
        hsAttemptPasswordChange();
    }

}

void MainWindow::hsAttemptPasswordChange()
{
    QString firstPassword{escapeAllControlCharacters(this->m_ui->hsPasswordLineEdit->text())};
    QString secondPassword{escapeAllControlCharacters(this->m_ui->hsSecondPasswordLineEdit->text())};

    if (firstPassword != secondPassword) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(NON_MATCHING_PASSWORD_STRING);
        errorBox->exec();
        this->m_ui->hsPasswordLineEdit->clear();
        this->m_ui->hsSecondPasswordLineEdit->clear();
        this->m_ui->hsPasswordLineEdit->setFocus();
    } else if (firstPassword.length() < PASSWORD_LENGTH_MINIMUM) {
        std::unique_ptr<QMessageBox> errorBox{std::make_unique<QMessageBox>(this)};
        errorBox->setText(PASSWORD_TOO_SHORT_STRING);
        errorBox->exec();
        this->m_ui->hsPasswordLineEdit->clear();
        this->m_ui->hsSecondPasswordLineEdit->clear();
        this->m_ui->hsCurrentPasswordLineEdit->setFocus();
    } /*else if (!containsUpperCaseCharacter(firstPassword)) {
        std::unique_ptr<QMessageBox> errorBox{new QMessageBox(this)};
        errorBox->setText(PASSWORD_DOES_NOT_CONTAIN_UPPER_CASE_STRING);
        errorBox->exec();
        ui->hsPasswordLineEdit->clear();
        ui->hsSecondPasswordLineEdit->clear();
    } */else {
        this->m_ui->hsPasswordLineEdit->clear();
        this->m_ui->hsSecondPasswordLineEdit->clear();

        this->m_hsSalt = "";
        this->m_hsHashedAndSaltedPassword = "";

        this->m_ui->hsGeneratedSaltLineEdit->clear();
        this->m_ui->hsGeneratedSaltyHashLineEdit->clear();

        while (this->m_hsSalt.length() != SALT_DIGIT_COUNT) {
            this->m_hsSalt.append(toQString(randomAsciiByte()));
        }
        this->m_hsHashedAndSaltedPassword = toQString(this->m_pythonCrypto->getSHA256Hash(this->m_hsSalt.toStdString() + firstPassword.toStdString()));

        if (this->m_hsPasswordSaved) {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(PASSWORD_CHANGE_SUCCESSFUL_STRING);
            successBox->exec();
        } else {
            this->m_ui->statusBar->showMessage(INITIAL_PASSWORD_STORED_STRING, INITIAL_PASSWORD_STORED_TIMEOUT);
            std::unique_ptr<QMessageBox> successBox{std::make_unique<QMessageBox>(this)};
            successBox->setText(INITIAL_PASSWORD_STORED_STRING);
            successBox->exec();
        }

        this->m_hsPasswordSaved = true;

        this->m_ui->hsGeneratedSaltLineEdit->setText(this->m_hsSalt);
        this->m_ui->hsGeneratedSaltyHashLineEdit->setText(this->m_hsHashedAndSaltedPassword);
        this->m_ui->hsCurrentPasswordLineEdit->setEnabled(true);
        this->m_ui->hsCurrentPasswordLineEdit->setFocus();

        this->m_ui->statusBar->showMessage(PASSWORD_CHANGE_SUCCESSFUL_STRING, PASSWORD_CHANGE_SUCCESSFUL_STRING_TIMEOUT);
    }
}

void MainWindow::onHsClearButtonClicked()
{
    this->m_ui->hsCurrentPasswordLineEdit->clear();
    this->m_ui->hsCurrentPasswordLineEdit->setEnabled(false);

    this->m_ui->hsPasswordLineEdit->clear();
    this->m_ui->hsSecondPasswordLineEdit->clear();

    this->m_ui->hsGeneratedSaltyHashLineEdit->clear();
    this->m_ui->hsGeneratedSaltLineEdit->clear();


    this->m_ui->hsPasswordLineEdit->setFocus();
    this->m_hsPasswordSaved = false;
    this->m_hsHashedAndSaltedPassword = "";
    this->m_hsSalt = "";
    this->m_ui->statusBar->showMessage(PASSWORD_CLEARED_STRING, PASSWORD_CLEARED_STRING_TIMEOUT);
}

bool MainWindow::containsUpperCaseCharacter(const QString &stringToCheck)
{
    std::string copyString{stringToCheck.toStdString()};
    for (auto &c : copyString) {
        if (isupper(c)) {
            return true;
        }
    }
    return false;
}

void MainWindow::onMessageChanged(QString newMessage)
{
    if (newMessage == "") {
        this->m_ui->statusBar->showMessage(PASSWORD_PROMPT_STRING);
    }
}

long MainWindow::QStringToLong(const QString &QStringToConvert)
{
    try {
        long returnLong{std::stol(QStringToConvert.toStdString())};
        return returnLong;
    } catch (std::exception &e) {
        Q_UNUSED(e);
        return -1;
    }
}

char MainWindow::randomAsciiByte()
{
    const int divisor{(RAND_MAX/ASCII_HIGH_LIMIT + 1)};
    int returnValue{ASCII_LOW_LIMIT - 1};
    do {
        returnValue = (rand() / divisor);
    }
    while ((returnValue > ASCII_HIGH_LIMIT) || (returnValue < ASCII_LOW_LIMIT) || containsAsciiControlCharacter(returnValue));
    return (char)returnValue;
}

bool MainWindow::containsAsciiControlCharacter(int v)
{
    for (auto &it : asciiControlCharacters) {
        if (v == it) {
            return true;
        }
    }
    return false;
}

MainWindow::~MainWindow()
{

}
