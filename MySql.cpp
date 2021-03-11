#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

sql::Driver *driver;
sql::Connection *con;
sql::Statement *stmt;
sql::PreparedStatement *pstmt;
sql::ResultSet *result;

const std::string SQserver = "tcp://127.0.0.1:3306";
const std::string username = "root";
const std::string password = "";

void INSERT_ACCOUNT(std::string growid, std::string pass, std::stringstream blob)
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SQserver, username, password);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        exit(1);
    }

    con->setSchema("gt");
    pstmt = con->prepareStatement("INSERT INTO players(username, password, data) VALUES(?,?,?)");
    pstmt->setString(1, growid);
    pstmt->setString(2, pass);
    pstmt->setBlob(3, &blob);
    pstmt->execute();

    delete pstmt;
    delete con;
}
void UPDATE(std::stringstream str, std::string growid)
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SQserver, username, password);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        exit(1);
    }

    con->setSchema("gt");

    //update
    pstmt = con->prepareStatement("UPDATE players SET data = ? WHERE username = ?");
    pstmt->setBlob(1, &str);
    pstmt->setString(2, growid);
    pstmt->executeQuery();

    delete con;
    delete pstmt;
}

bool ACCOUNT_EXIST(std::string growid)
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SQserver, username, password);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        exit(1);
    }
    bool exist = false;
    con->setSchema("gt");

    pstmt = con->prepareStatement("SELECT * FROM players WHERE username = ?");
    pstmt->setString(1, growid);
    result = pstmt->executeQuery();

    while (result->next())
    {
        if (result->getString(2).c_str() == growid)
        {
            exist = true;
        }
    }

    delete result;
    delete pstmt;
    delete con;

    return exist;
}

bool CHECK_LOGIN(std::string growid, std::string pass)
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SQserver, username, password);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        exit(1);
    }
    bool correct = false;
    con->setSchema("gt");

    pstmt = con->prepareStatement("SELECT * FROM players WHERE username = ?");
    pstmt->setString(1, growid);
    result = pstmt->executeQuery();

    while (result->next())
    {
        if (result->getString(2).c_str() == growid && result->getString(3).c_str() == pass)
        {
            correct = true;
        }
    }

    delete result;
    delete pstmt;
    delete con;

    return correct;
}

std::istream* UPDATE_DATA(std::string growid)
{
    try
    {
        driver = get_driver_instance();
        con = driver->connect(SQserver, username, password);
    }
    catch (sql::SQLException &e)
    {
        std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
        exit(1);
    }
    std::istream *blobdata;
    con->setSchema("gt");

    pstmt = con->prepareStatement("SELECT * FROM players WHERE username = ?");
    pstmt->setString(1, growid);
    result = pstmt->executeQuery();

    while (result->next())
    {
        if (result->getString(2).c_str() == growid)
        {
            blobdata = result->getBlob(4);
        }
    }

    delete result;
    delete pstmt;
    delete con;

    return blobdata;
}
