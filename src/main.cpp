#include "debug/log.hpp"
#include "game.hpp"
#include "result.hpp"

int main()
{
#ifdef NDEBUG
    initLog(LOG_LEVEL_ERROR, LOG_LEVEL_WARN);
#else
    initLog(LOG_LEVEL_DEBUG, LOG_LEVEL_TRACE);
#endif

    Game* game = new Game();

    Result result = game->run();

    if (result != RESULT_SUCCESS)
    {
        LOG_CRITICAL("The game exited due to an error.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
