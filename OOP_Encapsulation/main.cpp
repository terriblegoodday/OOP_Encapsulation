//
//  main.cpp
//  OOP_Encapsulation
//
//  Created by Eduard Dzhumagaliev on 5/1/20.
//  Copyright © 2020 Eduard Dzhumagaliev. All rights reserved.
//

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <iomanip>
#include <numeric>

using namespace std;

struct Point {
    // public properties
    // can it be considered an abstraction (as data structure)?
    int x;
    int y;
};

struct StatsDelta {
    // No encapsulation
    int deltaLevel;
    int deltaHealth;
};
struct StatsDeltaMultiply {
    // No encapsulation
    float multiplyLevel;
    float multiplyHealth;
};

class Buff {
    // Encapsulation with Getters
private:
    // Class Invariant
    const uint MAX_DESCRIPTION_LENGTH = 30;
    const uint MAX_BUFF_SUM = 10;
    
    StatsDeltaMultiply buffEffect;
    
    string description;
    
public:
    Buff(StatsDeltaMultiply buffEffect, string description) {
        uint descriptionLength = (uint)description.length();
        if (descriptionLength > MAX_DESCRIPTION_LENGTH) throw domain_error("Buff description length greater than max");
        if (buffEffect.multiplyLevel + buffEffect.multiplyHealth > 10) throw domain_error("Buff sum greater than max");
        this->description = description;
        this->buffEffect = buffEffect;
    };
    
    string getDescription() const {
        return this->description;
    };
    
    friend bool operator==(const Buff & lhs, const Buff & rhs) {
        return lhs.description == rhs.description;
    };
    
    friend bool operator<(const Buff & lhs, const Buff & rhs) {
        return lhs.description.length() < rhs.description.length();
    }
};
typedef set<Buff> BuffSet;
struct BuffsDelta {
    // No encapsulation
    BuffSet add;
    BuffSet remove;
};

struct PositionDelta {
    // No encapsulation
    int x;
    int y;
};
    
class ActionSubscriber {
    // Encapsulation with special protocols/methods
    // Abstract Class
public:
    // Late binding (dynamic binding)
    virtual void apply(StatsDelta & effects) = 0;
    virtual void apply(BuffsDelta & buffs) = 0;
    virtual void apply(PositionDelta & position) = 0;
    virtual string getSubscriberDescription() = 0;
};

class Player: public ActionSubscriber {
private:
    // Class invariant
    const uint MAX_NICKNAME_LENGTH = 30;
    const uint MAX_LEVEL = 256;
    const uint MAX_HEALTH = 1000;
    
    string nickname;
    
    uint level;
    uint health;
    
    BuffSet buffs;
    
    Point currentPosition;
    Point checkPoint;
    
    void removeBuffs(BuffSet & buffs) {
        BuffSet buffIntersection;
        set_intersection(this->buffs.begin(), this->buffs.end(), buffs.begin(), buffs.end(), inserter(buffIntersection, buffIntersection.end()));
        this->buffs.erase(buffIntersection.begin(), buffIntersection.end());
    };
    
    void addBuffs(BuffSet & buffs) {
        this->buffs.insert(buffs.begin(), buffs.end());
    };
    
public:
    Player(string nickname,
           uint level,
           Point currentPosition) {
        uint nicknameLength = (uint)nickname.length();
        if (nicknameLength > MAX_NICKNAME_LENGTH) throw domain_error("Nickname length greater than max");
        if (level > MAX_LEVEL) throw domain_error("Level greater than max");
        this->nickname = nickname;
        this->level = level;
        this->health = 1000;
        this->currentPosition = currentPosition;
        this->checkPoint = currentPosition;
    };
    
    // Getters
    Point getCurrentPosition() { return currentPosition; };
    uint getLevel() { return level; };
    uint getHealth() { return health; };
    string getNickname() { return nickname; };
    string getBuffs() { // Encapsulation with special methods
        string buffsDescriptor = "";
        for (const Buff & buff: buffs) {
            buffsDescriptor += buff.getDescription();
        };
        return buffsDescriptor;
    };
    string getDescription() {
        Point currentPosition = getCurrentPosition();
        return "🤫 " + getNickname() + " " + to_string(getLevel()) + " " + to_string(getHealth()) +
        " (" + to_string(currentPosition.x) + ", " + to_string(currentPosition.y) + ") " + getBuffs();
    };
    
    // Setters
    void setNickname(string nickname) {
        uint nicknameLength = (uint)nickname.length();
        if (nicknameLength > MAX_NICKNAME_LENGTH) throw domain_error("Nickname length greater than max. Expected " + to_string(MAX_NICKNAME_LENGTH) + ", got " + to_string(nicknameLength));
        this->nickname = nickname;
    }
    
    // ActionSubscriber
    void apply(StatsDelta & stats) override {
        level = (level + stats.deltaLevel) % (MAX_LEVEL + 1);
        health = min((uint)0, health + stats.deltaHealth) % (MAX_HEALTH + 1);
    };
    void apply(BuffsDelta & buffs) override {
        removeBuffs(buffs.remove);
        addBuffs(buffs.add);
    };
    void apply(PositionDelta & positionDelta) override {
        currentPosition.x += positionDelta.x;
        currentPosition.y += positionDelta.y;
    }
    string getSubscriberDescription() override {
        return getNickname();
    }
};

class Action {
    // Encapsulation by abstraction (using late binding)
public:
    virtual void execute() = 0;
    virtual string getDescription() = 0;
};

class Move: public Action {
private:
    const int RAND_LIMIT = 12;
    
protected:
    PositionDelta moveDelta;
    ActionSubscriber * subscriber;
    
public:
    Move(ActionSubscriber & subscriber) {
        this->subscriber = &subscriber;
    };
    
    void execute() override {
        cout << "🚶🏻‍♂️ Moved player " + subscriber->getSubscriberDescription() << endl;
        
        int random1 = rand() % RAND_LIMIT;
        int random2 = rand() % RAND_LIMIT;
        moveDelta = {.x = random1, .y = random2};
        
        int direction = random1 % 3;
        BuffsDelta buffsDelta;
        if (direction == 0) {
            cout << subscriber->getSubscriberDescription() << " found magical herb in the Himalayas 🌿" << endl;
            StatsDeltaMultiply herb = {.multiplyHealth = 1.5, .multiplyLevel = 1.5};
            buffsDelta.add.insert(Buff(herb, "🌿"));
        } else if (direction == 1) {
            cout << subscriber->getSubscriberDescription() << " ordered coffee ☕" << endl;
            StatsDeltaMultiply coffee = {.multiplyHealth = 1.3, .multiplyLevel = 1.3};
            buffsDelta.add.insert(Buff(coffee, "☕"));
        };
        subscriber->apply(buffsDelta);
        subscriber->apply(moveDelta);
    }
    
    string getDescription() override {
        return "🚶🏻‍♂️ Move player " + subscriber->getSubscriberDescription();
    }
};

class Jog: public Move {
    // Late binding
    // Behaviour **override**
private:
    const uint RAND_LIMIT = 24;
    
public:
    Jog(ActionSubscriber & subscriber): Move(subscriber) {};
    
    void execute() override {
        cout << "🏃 Player's running " + subscriber->getSubscriberDescription() << endl;
        
        int random1 = rand() % RAND_LIMIT;
        int random2 = rand() % RAND_LIMIT;
        
        moveDelta = {.x = random1, .y = random2};
        
        subscriber->apply(moveDelta);
    }
    
    string getDescription() override {
        return "🏃 Run player " + subscriber->getSubscriberDescription();
    }
};

class MountainJog: public Jog {
    // Late binding
    // Behaviour **extension**
public:
    MountainJog(ActionSubscriber & subscriber): Jog(subscriber) {};
    
    void execute() override {
        Jog::execute();
        cout << " He/she seems to be running over the mountains!" << endl;
    };
    
    string getDescription() override {
        return "⛰️ Mountain run player " + subscriber->getSubscriberDescription();
    }
};

class AdminSuicide: public Action {
private:
    ActionSubscriber * admin;
    
public:
    AdminSuicide(ActionSubscriber & admin) {
        this->admin = &admin;
    }
    
    void execute() override {
        bool willDie = rand() % 2;
        if (willDie) {
            cout << "gg 🙂" << endl;
            StatsDelta suicide = {.deltaHealth = -10000, .deltaLevel = 0};
            this->admin->apply(suicide);
        } else {
            cout << "uh oh 🤔 your potion didn't work so you didn't die but you'll level up much slower; you can try this one again 😋" << endl;
            BuffsDelta buffsDelta;
            StatsDeltaMultiply badPotion = {.multiplyLevel = 0.1, .multiplyHealth = 1.0};
            buffsDelta.add.insert(Buff(badPotion, "🧪⚰️"));
            this->admin->apply(buffsDelta);
        }
    };
    
    string getDescription() override {
        return "💀 Admin Suicide";
    }
};

class Game {
private:
    uint playersCount;
    vector<Player*> onlinePlayers;
    Player * admin;
    
    map<string, Action*> availableActions;
    
    void printPlayerStats() {
        for (Player * player: onlinePlayers) {
            cout << player->getDescription() << endl;
        }
    }
    
    void printAvailableActions() {
        cout << "Available Actions:" << endl;
        for (const auto & action: availableActions) {
            cout << left << setw(25) << setfill(' ') << action.first << ": ";
            cout << action.second->getDescription() << endl;
        }
    }
    
    void startGameLoop() {
        cout << "🤡 Welcome to the game of Life. To quit the game enter `q`." << endl;
        while(admin->getHealth() > 0) {
            printPlayerStats();
            printAvailableActions();
            cout << "> ";
            string input;
            cin >> input;
            if (availableActions.count(input) > 0) {
                // Encapsulation by abstraction (using late binding)
                availableActions[input]->execute();
            };
        }
    }
    
    void printGameResults() {
        cout << "Game Results: " << endl;
        printPlayerStats();
    }
    
public:
    Game(Player & admin) {
        this->admin = &admin;
        this->onlinePlayers.push_back(&admin);
        
        this->availableActions["/q"] = new AdminSuicide(*this->admin);
        this->availableActions["/m"] = new Move(*this->admin);
    }
    
    void add(Player & player) {
        this->availableActions["/move:" + player.getNickname()] = new Move(player);
        this->availableActions["/jog:" + player.getNickname()] = new Jog(player);
        this->availableActions["/mjog:" + player.getNickname()] = new MountainJog(player);
        onlinePlayers.push_back(&player);
    }
    
    void start() {
        startGameLoop();
        printGameResults();
    }
};

int main(int argc, const char * argv[]) {
    Player admin("aldrt23", 32, (Point){.x = 3, .y = 2});
    Game game(admin);
    Player player2("fersp63", 16, (Point){.x = 6, .y = 7});
    game.add(player2);
    game.start();
    return 0;
}
