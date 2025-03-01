#include "async.h"

namespace async {

handle_t connect(std::size_t bulk) {
    handle_t res = contexts.size();
    IReaderPtr ip = IReaderPtr{new YieldReader()};
    contexts[res] = std::unique_ptr<Application>(new Application(bulk, std::move(ip)));
    ILoggerPtr cLogger(new ConsoleLogger());
    ILoggerPtr fLogger(new FileLogger());
    std::string logger_id = std::to_string(res);
    fLogger->set_id(logger_id);
    contexts[res]->subscribeLogger(cLogger);
    contexts[res]->subscribeLogger(fLogger);
    return res;
}

void receive(handle_t h, const char * data, std::size_t sz) {
    if (!data || sz == 0) 
        return;
    if(contexts.find(h) == contexts.end())
        return;
    std::string message(static_cast<const char*>(data), sz);
    contexts[h]->pushStringToReader(message);
}

void disconnect(handle_t h) {
    if(contexts.find(h) != contexts.end())
    {
        contexts[h]->terminate();
        contexts.erase(h);
    }
}

}
