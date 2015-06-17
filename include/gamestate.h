#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

class GameState
{
    private:

    public:
        GameState();
        virtual ~GameState();
        enum State {ST_STARTING, ST_PLAYING, ST_BOSS, ST_END};
        State currentState;
};

#endif // GAME_H_INCLUDED
