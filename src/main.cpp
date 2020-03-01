#include "debug/log.hpp"
#include "game.hpp"

int main()
{
    initLog(LOG_LEVEL_DEBUG, LOG_LEVEL_TRACE);

    Game* game = new Game();

    if (auto result = game->run())
    {
        LOG_CRITICAL("The game exited due to an error: {}", result.value());
        return -1;
    }

    return 0;
}
