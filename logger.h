#pragma once
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include "loader_manager.h"
#include <mutex>

#define CONSOLE_API

namespace FractureLogger
{
	enum class Color
	{
		RED = 12,
		GREEN = 10,
		YELLOW = 14,
		BLUE = 9,
		MAGENTA = 13,
		CYAN = 11,
		WHITE = 15,
		GRAY = 8,
		DARK_RED = 4,
		DARK_GREEN = 2
	};

	enum class LogLevel
	{
		DEBUG,
		INFO,
		WARN,
		ERR
	};

	class CONSOLE_API Logger
	{
		static HANDLE hConsole;
		static bool consoleAllocated;
		static std::ofstream logFile;
		static std::string logFilePath;
		static bool fileLoggingInitialized;
		static std::string getTimestamp();
		static std::string getFileTimestamp();
		static std::string getLevelString(LogLevel level);
		static Color getLevelColor(LogLevel level);
		static std::mutex logMutex;
		static bool initializeFileLogging();
		static void ensureLogsDirectory();
		static std::string generateLogFileName();

	public:
		static bool allocConsole();
		static void freeConsole();
		static void closeLogFile();

		static void setColor(Color color);
		static void resetColor();

		template <typename... Args>
		static void log(LogLevel level, const std::string& format, Args&&... args);

		template <typename... Args>
		static void msg(const std::string& format, Args&&... args)
		{
			log(LogLevel::INFO, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void error(const std::string& format, Args&&... args)
		{
			log(LogLevel::ERR, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void debug(const std::string& format, Args&&... args)
		{
			log(LogLevel::DEBUG, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void warn(const std::string& format, Args&&... args)
		{
			log(LogLevel::WARN, format, std::forward<Args>(args)...);
		}

		template <typename... Args>
		static void logArgsWhite(LogLevel level, const std::string& format, Args&&... args);

		static void msg(const std::string& message);
		static void error(const std::string& message);
		static void debug(const std::string& message);
		static void warn(const std::string& message);
	};

	template <typename... Args>
	void Logger::log(LogLevel level, const std::string& format, Args&&... args)
	{
		std::lock_guard<std::mutex> lock(logMutex);

		if (!consoleAllocated)
		{
			allocConsole();
		}

		if (!fileLoggingInitialized)
		{
			initializeFileLogging();
		}

		std::ostringstream oss;

		if constexpr (sizeof...(args) > 0)
		{
			size_t pos = 0;
			std::string result = format;

			auto replaceNext = [&](auto&& arg)
			{
				size_t startPos = result.find("{", pos);
				if (startPos != std::string::npos)
				{
					size_t endPos = result.find("}", startPos);
					if (endPos != std::string::npos)
					{
						std::string placeholder = result.substr(startPos, endPos - startPos + 1);
						std::ostringstream temp;

						if (placeholder.find(":X") != std::string::npos || placeholder.find(":x") != std::string::npos)
						{
							temp << "0x" << std::hex << std::uppercase << arg;
						}
						else
						{
							temp << arg;
						}

						result.replace(startPos, endPos - startPos + 1, temp.str());
						pos = startPos + temp.str().length();
					}
				}
			};

			(replaceNext(args), ...);
			oss << result;
		}
		else
		{
			oss << format;
		}

		std::string logMessage = oss.str();

		setColor(getLevelColor(level));

		setColor(Color::GRAY);
		std::cout << "[" << getTimestamp() << "] ";

		setColor(Color::CYAN);
		std::cout << "[FractureLoader] ";

		setColor(getLevelColor(level));
		std::cout << "[" << getLevelString(level) << "] ";

		setColor(Color::WHITE);
		std::cout << logMessage << std::endl;

		resetColor();

		if (logFile.is_open())
		{
			logFile << "[" << getFileTimestamp() << "] [FractureLoader] [" << getLevelString(level) << "] "
				<< logMessage << std::endl;
			logFile.flush();
		}
	}

	inline bool Logger::allocConsole()
	{
		if (consoleAllocated) return true;

		if (!AllocConsole())
		{
			return false;
		}

		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

		std::ios::sync_with_stdio(true);

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleAllocated = true;

		SetConsoleTitleA("FractureLoader");
		SetConsoleOutputCP(CP_UTF8);

		setColor(Color::CYAN);
		std::cout << R"(
  _________________________________________

              FractureLoader                
               Initialized              
  _________________________________________
)" << std::endl;
		resetColor();

		return true;
	}

	inline void Logger::freeConsole()
	{
		if (consoleAllocated)
		{
			FreeConsole();
			consoleAllocated = false;
			hConsole = nullptr;
		}
	}

	inline void Logger::closeLogFile()
	{
		std::lock_guard<std::mutex> lock(logMutex);
		if (logFile.is_open())
		{
			logFile.close();
		}
		fileLoggingInitialized = false;
	}

	inline std::string Logger::getTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()) % 1000;

		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
		oss << "." << std::setfill('0') << std::setw(3) << ms.count();
		return oss.str();
	}

	inline std::string Logger::getFileTimestamp()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()) % 1000;

		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
		oss << "." << std::setfill('0') << std::setw(3) << ms.count();
		return oss.str();
	}

	inline void Logger::setColor(Color color)
	{
		if (hConsole)
		{
			SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
		}
	}

	inline void Logger::resetColor()
	{
		if (hConsole)
		{
			SetConsoleTextAttribute(hConsole, static_cast<WORD>(Color::WHITE));
		}
	}

	inline std::string Logger::getLevelString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::DEBUG: return "DEBUG";
		case LogLevel::INFO: return "INFO";
		case LogLevel::WARN: return "WARN";
		case LogLevel::ERR: return "ERROR";
		default: return "UNKNOWN";
		}
	}

	inline Color Logger::getLevelColor(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::DEBUG: return Color::GRAY;
		case LogLevel::INFO: return Color::GREEN;
		case LogLevel::WARN: return Color::YELLOW;
		case LogLevel::ERR: return Color::RED;
		default: return Color::WHITE;
		}
	}

	inline void Logger::ensureLogsDirectory()
	{
		if (g_loaderManager)
		{
			std::string logsDir = g_loaderManager->getBaseDirectory() + "\\logs";
			try
			{
				std::filesystem::create_directories(logsDir);
			}
			catch (const std::exception&)
			{
				// we silently fail if we can't create the directory
			}
		}
	}

	inline std::string Logger::generateLogFileName()
	{
		auto now = std::chrono::system_clock::now();
		auto time_t = std::chrono::system_clock::to_time_t(now);

		std::ostringstream oss;
		oss << std::put_time(std::localtime(&time_t), "FractureLoader_%Y%m%d_%H%M%S.txt");
		return oss.str();
	}

	inline bool Logger::initializeFileLogging()
	{
		if (fileLoggingInitialized) return true;

		if (!g_loaderManager) return false;

		ensureLogsDirectory();

		std::string logsDir = g_loaderManager->getBaseDirectory() + "\\logs";
		std::string fileName = generateLogFileName();
		logFilePath = logsDir + "\\" + fileName;

		logFile.open(logFilePath, std::ios::out | std::ios::app);
		if (logFile.is_open())
		{
			fileLoggingInitialized = true;

			logFile << "===========================================\n";
			logFile << "FractureLoader Log Session Started\n";
			logFile << "Timestamp: " << getFileTimestamp() << "\n";
			logFile << "===========================================\n\n";
			logFile.flush();

			return true;
		}

		return false;
	}

	template <typename... Args>
	void Logger::logArgsWhite(LogLevel level, const std::string& format, Args&&... args)
	{
		std::lock_guard<std::mutex> lock(logMutex);

		if (!consoleAllocated)
			allocConsole();

		if (!fileLoggingInitialized)
		{
			initializeFileLogging();
		}

		setColor(getLevelColor(level));

		setColor(Color::GRAY);
		std::cout << "[" << getTimestamp() << "] ";

		setColor(Color::CYAN);
		std::cout << "[FractureLoader] ";

		setColor(getLevelColor(level));
		std::cout << "[" << getLevelString(level) << "] ";

		size_t pos = 0;
		std::string result = format;

		std::vector<std::string> parts;
		std::vector<std::string> literals;
		size_t last = 0;
		while ((pos = result.find('{', last)) != std::string::npos)
		{
			size_t end = result.find('}', pos);
			if (end != std::string::npos)
			{
				literals.push_back(result.substr(last, pos - last));
				parts.push_back(result.substr(pos, end - pos + 1));
				last = end + 1;
			}
			else
			{
				break;
			}
		}
		literals.push_back(result.substr(last));

		std::ostringstream argsStream;
		auto insertArg = [&](auto&& arg)
		{
			std::ostringstream temp;
			temp << arg;
			return temp.str();
		};

		std::vector<std::string> argStrs = {insertArg(std::forward<Args>(args))...};

		std::ostringstream completeMessage;

		for (size_t i = 0; i < literals.size(); ++i)
		{
			setColor(getLevelColor(level));
			std::cout << literals[i];
			completeMessage << literals[i];

			if (i < argStrs.size())
			{
				setColor(Color::WHITE);
				std::cout << argStrs[i];
				completeMessage << argStrs[i];
			}
		}

		std::cout << std::endl;
		resetColor();

		if (logFile.is_open())
		{
			logFile << "[" << getFileTimestamp() << "] [FractureLoader] [" << getLevelString(level) << "] "
				<< completeMessage.str() << std::endl;
			logFile.flush();
		}
	}

	inline void Logger::msg(const std::string& message)
	{
		log(LogLevel::INFO, message);
	}

	inline void Logger::error(const std::string& message)
	{
		log(LogLevel::ERR, message);
	}

	inline void Logger::debug(const std::string& message)
	{
		if (!g_loaderManager->getValue<bool>("debugLog", true))
		{
			return;
		}

		log(LogLevel::DEBUG, message);
	}

	inline void Logger::warn(const std::string& message)
	{
		log(LogLevel::WARN, message);
	}
}

#define FRACTURE_ALLOC_CONSOLE() FractureLogger::Logger::allocConsole()
#define FRACTURE_FREE_CONSOLE() FractureLogger::Logger::freeConsole()
#define FRACTURE_CLOSE_LOG_FILE() FractureLogger::Logger::closeLogFile()
#define FRACTURE_LOG(level, ...) FractureLogger::Logger::log(level, __VA_ARGS__)
#define FRACTURE_MSG(...) FractureLogger::Logger::msg(__VA_ARGS__)
#define FRACTURE_ERROR(...) FractureLogger::Logger::error(__VA_ARGS__)
#define FRACTURE_DEBUG(...) FractureLogger::Logger::debug(__VA_ARGS__)
#define FRACTURE_WARN(...) FractureLogger::Logger::warn(__VA_ARGS__)
#define FRACTURE_SETLOGCOLOR(color) FractureLogger::Logger::setColor(color)
#define FRACTURE_RESETLOGCOLOR() FractureLogger::Logger::resetColor()
#define FRACTURE_MSG_ARGS_WHITE(fmt, ...) \
    FractureLogger::Logger::logArgsWhite(FractureLogger::LogLevel::INFO, fmt, __VA_ARGS__)

inline HANDLE FractureLogger::Logger::hConsole = nullptr;
inline bool FractureLogger::Logger::consoleAllocated = false;
inline std::ofstream FractureLogger::Logger::logFile;
inline std::string FractureLogger::Logger::logFilePath;
inline bool FractureLogger::Logger::fileLoggingInitialized = false;
inline std::mutex FractureLogger::Logger::logMutex;
