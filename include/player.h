#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

class Player
{
    private:
        double speedForward;

    public:
        Player(double speedForward);
        virtual ~Player();

        double getSpeedForward();

        void setSpeedForward(double speedForward);
};

#endif // PLAYER_H_INCLUDED
