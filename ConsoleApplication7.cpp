#include <gtest/gtest.h> 
#include <iostream>
#include <string>
#include <gmock/gmock.h> 

class IDBConnection
{
public:
	virtual ~IDBConnection() = default; 
	virtual void open() = 0; 
	virtual void close() = 0; 
	virtual std::string execQuery(const std::string& query) = 0;
};

class DBConnection : public IDBConnection
{
public:
	virtual void open() override {}
	virtual void close() override {}
	virtual std::string execQuery(const std::string& query) override
	{
		return "real result";
	}
}; 

class ClassThatUsesDB
{
private:
	IDBConnection* db_;
public:
	explicit ClassThatUsesDB (IDBConnection* db) : db_(db) {}

	void openConnection() 
	{
		db_->open();
	}
	std::string useConnection(const std::string& query)
	{
		return db_->execQuery(query); 
	}
	void closeConnection()
	{
		db_->close(); 
	}
};

class MockDBConnection : public IDBConnection {
public:
	MOCK_METHOD(void, open, (), (override));
	MOCK_METHOD(void, close, (), (override));
	MOCK_METHOD(std::string, execQuery, (const std::string& query), (override));
};

using ::testing::Return; 
using ::testing::_;
using ::testing::InSequence; 

TEST(ClassThatUsesDBTest, testcase1) 
{
	MockDBConnection mockDB;
	EXPECT_CALL(mockDB, open()).Times(1); 

	ClassThatUsesDB testObj(&mockDB);
	testObj.openConnection();
}
TEST(ClassThatUsesDBTest, testcase2)
{
	MockDBConnection mockDB;
	std::string query = "Select * from users";
	std::string expectedResult = "mocked result";

	EXPECT_CALL(mockDB, execQuery(query)).WillOnce(Return(expectedResult));

	ClassThatUsesDB testObj(&mockDB);
	std::string result = testObj.useConnection(query); 

	EXPECT_EQ(result, expectedResult); 
}

TEST(ClassThatUsesDBTest, testcase3) 
{
	MockDBConnection mockDB;
	EXPECT_CALL(mockDB, close()).Times(1);

	ClassThatUsesDB testObj(&mockDB);
	testObj.closeConnection();
}


TEST(ClassThatUsesDBTest, testcase4)
{
	MockDBConnection mockDB;
	InSequence s; 

	EXPECT_CALL(mockDB, open());  
	EXPECT_CALL(mockDB, execQuery("Select")).WillOnce(Return("result"));
	EXPECT_CALL(mockDB, close()); 

	ClassThatUsesDB testObj(&mockDB);
	testObj.openConnection();
	std::string result = testObj.useConnection("Select");
	testObj.closeConnection();

	EXPECT_EQ(result, "result"); 
}

int main(int argc, char* argv[]) 
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}