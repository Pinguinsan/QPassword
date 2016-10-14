#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <pythoncrypto.h>
#include <generalutilities.h>


#if defined(_WIN32) || defined(__MINGW32__)

#endif


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    const static QString MAIN_WINDOW_TITLE_STRING;
    const static QString MAIN_WINDOW_ICON_PATH;

private:
    const static QString NON_MATCHING_PASSWORD_STRING;
    const static QString PASSWORD_PROMPT_STRING;
    const static QString CURRENT_PASSWORD_INCORRECT_STRING;
    const static QString INITIAL_PASSWORD_STORED_STRING;
    const static QString PASSWORD_CHANGE_SUCCESSFUL_STRING;
    const static QString PASSWORD_CLEARED_STRING;
    const static QString PASSWORD_TOO_SHORT_STRING;
    const static QString PASSWORD_DOES_NOT_CONTAIN_UPPER_CASE_STRING;
    const static QString PYTHON_NOT_INSTALLED_STRING;
    const static int PASSWORD_CHANGE_SUCCESSFUL_STRING_TIMEOUT;
    const static int INITIAL_PASSWORD_STORED_TIMEOUT;
    const static int PASSWORD_CLEARED_STRING_TIMEOUT;
    const static int PASSWORD_LENGTH_MINIMUM;
    const static int ASCII_LOW_LIMIT;
    const static int ASCII_HIGH_LIMIT;
    const static int SALT_DIGIT_COUNT;

    bool m_ptPasswordSaved;
    QString m_ptPassword;

    bool m_hPasswordSaved;
    QString m_hHashedPassword;

    bool m_hsPasswordSaved;
    QString m_hsSalt;
    QString m_hsHashedAndSaltedPassword;

    std::unique_ptr<PythonCrypto> m_pythonCrypto;
    std::unique_ptr<Ui::MainWindow> m_ui;

    void ptAttemptPasswordChange();
    void hAttemptPasswordChange();
    void hsAttemptPasswordChange();

    bool containsUpperCaseCharacter(const QString &stringToCheck);


    template <typename T>
    QString toQString(const T &convert) { return QString::fromStdString(std::to_string(convert)); }
    QString toQString(const std::string &convert) { return QString::fromStdString(convert); }
    QString toQString(const char *convert) { return QString::fromStdString(static_cast<std::string>(convert)); }

    char randomAsciiByte();
    bool containsAsciiControlCharacter(int v);
    long QStringToLong(const QString &QStringToConvert);
    QString escapeAllControlCharacters(const QString &stringToEscape);

    const static std::vector<int> asciiControlCharacters;


private slots:
    void onPtSubmitButtonClicked();
    void onPtClearButtonClicked();

    void onHSubmitButtonClicked();
    void onHClearButtonClicked();

    void onHsSubmitButtonClicked();
    void onHsClearButtonClicked();

    void onMessageChanged(QString newMessage);
};

#endif // MAINWINDOW_H
