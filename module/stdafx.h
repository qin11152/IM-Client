#pragma once

#include "module/Log/Log.h"
#include "module/MyDefine.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/PublicDataManager/PublicDataManager.h"

#if defined(WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <string>
#include <string.h>
#include <unistd.h>
#endif

#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

#include <set>
#include <map>
#include <list>
#include <deque>
#include <tuple>
#include <mutex>
#include <atomic>
#include <future>
#include <memory>
#include <string>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <random>
#include <thread>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>

