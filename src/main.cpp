#include "game.hpp"
#include "debug/log.hpp"

int main()
{
#ifdef NDEBUG
    initLog(LOG_LEVEL_ERROR, LOG_LEVEL_WARN);
#else
    initLog(LOG_LEVEL_DEBUG, LOG_LEVEL_TRACE);
#endif

    Game* game = new Game();

    if (auto result = game->run())
    {
        LOG_CRITICAL("The game exited due to an error: {}", result.value());
        return -1;
    }

    return 0;
}