#include<iostream>
#include<iomanip>
#include<numeric>
#include<deque>
#include<vector>
#include<set>
#include<fstream>

int hour, minute;

void timePrint() {
    std::cout << std::setw(3) << std::setfill('0') << std::setiosflags(std::ios::right) << hour << ':' << std::setw(2)
              << std::setfill('0') << std::setiosflags(std::ios::right) << minute << ' ';
}

class CCreature;

class CWeapon;

class CFaction;

int nCity;

class CCity {
public:
    CFaction *_flag = NULL;
    CFaction *_lastWin = NULL;
    int _healthBonus = 0;
    CCreature *_redWarrior = NULL;
    CCreature *_blueWarrior = NULL;

    ~CCity() {
        if (_redWarrior != NULL)
            delete _redWarrior;
        if (_blueWarrior != NULL)
            delete _blueWarrior;
    }

    void raiseFlag(CFaction *faction);

};

CCity *city;


class CFaction {
    int _nWarrior = 0;
    std::deque<int> _generatingSeq;
public:
    int _quota;
    char _colour[8];
    int _colourID;
    CCity *_HQ;
    CFaction *_enemy;

    CFaction(const char str[8], int quota) : _quota(quota) {
        memcpy(_colour, str, 8);
        if (!strcmp(_colour, "red")) {
            _generatingSeq.push_back(2);
            _generatingSeq.push_back(3);
            _generatingSeq.push_back(4);
            _generatingSeq.push_back(1);
            _generatingSeq.push_back(0);
            _HQ = &city[0];
            _colourID = 0;
        } else {
            _generatingSeq.push_back(3);
            _generatingSeq.push_back(0);
            _generatingSeq.push_back(1);
            _generatingSeq.push_back(2);
            _generatingSeq.push_back(4);
            _HQ = &city[nCity + 1];
            _colourID = 1;
        }
    };

    CCreature *generateWarrior();
};

std::ostream &operator<<(std::ostream &out, CFaction &faction) {
    return out << faction._colour;
}

std::ostream &operator<<(std::ostream &out, CCreature &creature);


class CWeapon {
public:
    CCreature *_owner;

    CWeapon(CCreature *owner) : _owner(owner) {}

    char _typeName[8];
    int _type;

    virtual void worn() = 0;

    virtual void tryUse(CCreature *enemy, bool isAttcker) = 0;

    virtual void report() = 0;
};

class CCreature {
public:
    bool _isVirtual = false;
    int _power;
    int _health;
    int _ID;
    std::vector<CWeapon *> _weaponSet;
    CCity *_position;
    CFaction &_faction;

    CCreature(CFaction &faction, const int ID, const int type) : _faction(faction), _ID(ID), _position(faction._HQ),
                                                                 _type(type) {}

    CCreature &virtualize(CCity *virtualCity);

    void displayGenerateCreature(const char str[8]) {
        timePrint();
        std::cout << *this << " born" << std::endl;
    }

    bool operator<(const CCreature &rhs) const {
        return _ID < rhs._ID;
    }

    void assignWeapon(int weaponID);

    char _typeName[8];
    int _type;

    void removeWeapon(CWeapon *weapon) {
        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
            if (*it == weapon) {
                _weaponSet.erase(it);
                break;
            }
    }

    void fight(CCreature *enemy);

    void returnFight(CCreature *enemy);

    void tryBomb(CCreature *enemy);

    bool isDead() {
        return _health <= 0;
    }

    void hurt(int power) {
        _health -= power;
    }

    void reportWeapon() {
        timePrint();
        std::cout << *this << " has ";
        bool noWeapon = true;

        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++) {
            if ((*it)->_type != 2)continue;
            if (!noWeapon)std::cout << ',';
            noWeapon = false;
            (*it)->report();
        }
        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++) {
            if ((*it)->_type != 1)continue;
            if (!noWeapon)std::cout << ',';
            noWeapon = false;
            (*it)->report();
        }
        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++) {
            if ((*it)->_type != 0)continue;
            if (!noWeapon)std::cout << ',';
            noWeapon = false;
            (*it)->report();
        }
        if (noWeapon)std::cout << "no weapon";
        std::cout << std::endl;
    }

    void displayMarch();

    int attack(CCreature *enemy);

    int returnAttack(CCreature *enemy);


    void tryArrow(CCreature *enemy) {
        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
            if ((*it)->_type == 2) {
                (*it)->tryUse(enemy, 1);
                timePrint();
                std::cout << *this << " shot";
                if (enemy->isDead())
                    std::cout << " and killed " << *enemy;
                std::cout << std::endl;
                return;
            }
    }

    virtual~CCreature() {
        for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
            delete *it;
        if (_faction._colourID == 0)_position->_redWarrior = NULL;
        else _position->_blueWarrior = NULL;
    }
};

std::ostream &operator<<(std::ostream &out, CCreature &creature) {
    out << creature._faction << ' ' << creature._typeName << ' ' << creature._ID;
    return out;
}


class CDragon : public CCreature {
    double _morale;
public:
    friend class CFaction;

    CDragon(CFaction &faction, const int ID) : CCreature(faction, ID, 0) {
        _health = _initHealth;
        _power = _initPower;
        memcpy(_typeName, "dragon", 8);
        displayGenerateCreature(_typeName);
        _morale = (double) faction._quota / _initHealth;
        assignWeapon(_ID % 3);
        std::cout << "Its morale is " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << _morale
                  << std::endl;
    }

    void tryCheer(bool win) {
        if (win) _morale += 0.2;
        else _morale -= 0.2;
        if (_morale > 0.8) {
            timePrint();
            std::cout << *this << " yelled in city " << _position - city << std::endl;
        }
    }

    static int _initHealth;
    static int _initPower;
};

class CNinja : public CCreature {
public:
    friend class CFaction;

    CNinja(CFaction &faction, const int ID) : CCreature(faction, ID, 1) {
        _health = _initHealth;
        _power = _initPower;
        memcpy(_typeName, "ninja", 8);
        displayGenerateCreature(_typeName);
        assignWeapon(_ID % 3);
        assignWeapon((_ID + 1) % 3);
    }

    static int _initHealth;
    static int _initPower;
};

class CIceman : public CCreature {
    int _stepsTillMelt = 0;
public:
    friend class CFaction;

    CIceman(CFaction &faction, const int ID) : CCreature(faction, ID, 2) {
        _health = _initHealth;
        _power = _initPower;
        memcpy(_typeName, "iceman", 8);
        displayGenerateCreature(_typeName);
        assignWeapon(_ID % 3);
    }

    void tryMelt() {
        _stepsTillMelt++;
        if (_stepsTillMelt == 2) {
            _health -= 9;
            if (_health <= 0)_health = 1;
            _power += 20;
            _stepsTillMelt = 0;
        }
    };
    static int _initHealth;
    static int _initPower;
};

class CLion : public CCreature {
public:
    friend class CFaction;

    int _loyalty;

    CLion(CFaction &faction, const int ID) : CCreature(faction, ID, 3) {
        _health = _initHealth;
        _power = _initPower;
        memcpy(_typeName, "lion", 8);
        displayGenerateCreature(_typeName);
        _loyalty = faction._quota;
        std::cout << "Its loyalty is " << _loyalty << std::endl;
    }

    virtual void tryFlee() {
        if (_loyalty <= 0 && _position != _faction._enemy->_HQ) {
            timePrint();
            std::cout << *this << " ran away" << std::endl;
            delete this;
        }
    }

    static int _initHealth;
    static int _initPower;
    static int k;
};

int CLion::k;

class CWolf : public CCreature {
public:
    friend class CFaction;

    CWolf(CFaction &faction, const int ID) : CCreature(faction, ID, 4) {
        _health = _initHealth;
        _power = _initPower;
        memcpy(_typeName, "wolf", 8);
        displayGenerateCreature(_typeName);
    }

    void claimWeapon(CCreature *enemy) {
        bool changed = true;
        while (changed) {
            changed = false;
            for (std::vector<CWeapon *>::iterator it = enemy->_weaponSet.begin(); it != enemy->_weaponSet.end(); it++) {
                bool overlap = false;
                for (std::vector<CWeapon *>::iterator it1 = _weaponSet.begin(); it1 != _weaponSet.end(); it1++)
                    if ((*it)->_type == (*it1)->_type)overlap = true;
                if (!overlap) {
                    _weaponSet.push_back(*it);
                    (*it)->_owner = this;
                    enemy->_weaponSet.erase(it);
                    changed = true;
                    break;
                }
            }
        }
    }

    static int _initHealth;
    static int _initPower;
};

int CDragon::_initHealth;
int CNinja::_initHealth;
int CIceman::_initHealth;
int CLion::_initHealth;
int CWolf::_initHealth;

int CDragon::_initPower;
int CNinja::_initPower;
int CIceman::_initPower;
int CLion::_initPower;
int CWolf::_initPower;

class CSword : public CWeapon {
    friend class CCreature;

    int _power;
public:
    CSword(CCreature *owner) : CWeapon(owner), _power(_owner->_power / 5) {
        memcpy(_typeName, "sword", 8);
        _type = 0;
    }

    virtual void worn() {
        _power *= 0.8;
        if (!_power) {
            _owner->removeWeapon((CWeapon *) this);
            delete (this);
        }
    }

    virtual void tryUse(CCreature *enemy, bool isAttcker) {
        worn();
    }

    virtual void report() {
        std::cout << "sword(" << _power << ')';
    }
};


class CBomb : public CWeapon {
public:
    CBomb(CCreature *owner) : CWeapon(owner) {
        memcpy(_typeName, "bomb", 8);
        _type = 1;
    }

    virtual void worn() {
        _owner->removeWeapon((CWeapon *) this);
        delete (this);
    }

    virtual void tryUse(CCreature *enemy, bool isAttacker) {
        CCity virtualCity;
        CCreature &virtualOwner = _owner->virtualize(&virtualCity);

        CCreature &virtualEnemy = enemy->virtualize(&virtualCity);
        if (isAttacker)virtualOwner.fight(&virtualEnemy);
        else virtualEnemy.fight(&virtualOwner);
        if (virtualOwner.isDead()) {
            _owner->hurt(1E8);
            enemy->hurt(1E8);
            timePrint();
            std::cout << *_owner << " used a bomb and killed " << *enemy << std::endl;
        }
    }

    virtual void report() {
        std::cout << "bomb";
    }
};

class CArrow : public CWeapon {
    int _usage = 0;
public:
    CArrow(CCreature *owner) : CWeapon(owner) {
        memcpy(_typeName, "arrow", 8);
        _type = 2;
    }

    static int _power;

    virtual void tryUse(CCreature *enemy, bool isAttcker) {
        enemy->hurt(_power);
        worn();
    }

    virtual void worn() {
        _usage++;
        if (_usage == 3) {
            _owner->removeWeapon((CWeapon *) this);
            delete this;
        }
    }

    virtual void report() {
        std::cout << "arrow(" << 3 - _usage << ')';
    }
};

int CArrow::_power;

CCreature *CFaction::generateWarrior() {
    int cur = _generatingSeq.front();
    switch (cur) {
        case 0:
            if (_quota >= CDragon::_initHealth) {
                _quota -= CDragon::_initHealth;
                _nWarrior++;
                _generatingSeq.push_back(_generatingSeq.front());
                _generatingSeq.pop_front();
                return new CDragon(*this, _nWarrior);
            } else return NULL;
        case 1:
            if (_quota >= CNinja::_initHealth) {
                _quota -= CNinja::_initHealth;
                _nWarrior++;
                _generatingSeq.push_back(_generatingSeq.front());
                _generatingSeq.pop_front();
                return new CNinja(*this, _nWarrior);
            } else return NULL;
        case 2:
            if (_quota >= CIceman::_initHealth) {
                _quota -= CIceman::_initHealth;
                _nWarrior++;
                _generatingSeq.push_back(_generatingSeq.front());
                _generatingSeq.pop_front();
                return new CIceman(*this, _nWarrior);
            } else return NULL;
        case 3:
            if (_quota >= CLion::_initHealth) {
                _quota -= CLion::_initHealth;
                _nWarrior++;
                _generatingSeq.push_back(_generatingSeq.front());
                _generatingSeq.pop_front();
                return new CLion(*this, _nWarrior);
            } else return NULL;
        case 4:
            if (_quota >= CWolf::_initHealth) {
                _quota -= CWolf::_initHealth;
                _nWarrior++;
                _generatingSeq.push_back(_generatingSeq.front());
                _generatingSeq.pop_front();
                return new CWolf(*this, _nWarrior);
            } else return NULL;
    }
    return NULL;
}

void CCreature::assignWeapon(int weaponID) {
    switch (weaponID) {
        case 0: {
            if (_power >= 5)
                _weaponSet.push_back(new CSword(this));
            break;
        }
        case 1: {
            _weaponSet.push_back(new CBomb(this));
            break;
        }
        case 2: {
            _weaponSet.push_back(new CArrow(this));
            break;
        }
    };
}

CCity *nextCity(CCreature &creature) {
    if (creature._faction._colourID == 0)
        if (creature._position != &city[nCity + 1])
            return creature._position + 1;
        else return NULL;
    else if (creature._position != &city[0])
        return creature._position - 1;
    else return NULL;
}

CCreature &CCreature::virtualize(CCity *virtualCity) {
    CCreature *cur = new CCreature(*this);
    CWeapon *sword = NULL;
    for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
        if ((*it)->_type == 0)sword = new CSword(*(CSword *) *it);
    cur->_weaponSet.clear();
    if (sword != NULL)cur->_weaponSet.push_back(sword);
    cur->_isVirtual = true;
    cur->_position = virtualCity;
    return *cur;
}

void CCreature::tryBomb(CCreature *enemy) {
    for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
        if ((*it)->_type == 1) {
            ((CBomb *) *it)->tryUse(enemy, 1);
            if (isDead())return;
        }

    for (std::vector<CWeapon *>::iterator it = enemy->_weaponSet.begin(); it != enemy->_weaponSet.end(); it++)
        if ((*it)->_type == 1) {
            ((CBomb *) *it)->tryUse(this, 0);
            if (isDead())return;
        }
}

void CCreature::fight(CCreature *enemy) {

    enemy->hurt(attack(enemy));

    if (!_isVirtual) {
        timePrint();
        std::cout << *this << " attacked " << *enemy << " in city " << _position - city << " with " << _health
                  << " elements and force " << _power << std::endl;
    }
    if (!enemy->isDead())enemy->returnFight(this);
    if (enemy->isDead() && !_isVirtual) {
        timePrint();
        std::cout << *enemy << " was killed in city " << _position - city << std::endl;
    }
    if (!isDead() && _type == 3 && !enemy->isDead())((CLion *) this)->_loyalty -= CLion::k;
}

void CCreature::displayMarch() {
    timePrint();
    CCity *next = nextCity(*this);
    if (next != this->_faction._enemy->_HQ)
        std::cout << *this << " marched to city " << next - city << " with " << _health << " elements and force "
                  << _power << std::endl;
    else {
        std::cout << *this << " reached " << _faction._enemy->_colour << " headquarter with " << _health
                  << " elements and force " << _power << std::endl;
        if ((_faction._colourID == 0 && next->_redWarrior != NULL) ||
            (_faction._colourID == 1 && next->_blueWarrior != NULL)) {
            timePrint();
            std::cout << *(_faction._enemy) << " headquarter was taken" << std::endl;
        }
    }
}

int CCreature::attack(CCreature *enemy) {
    int n = _power;
    for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
        if ((*it)->_type == 0) {
            n += ((CSword *) *it)->_power;
            (*it)->tryUse(enemy, 1);
            break;
        }
    return n;
}

int CCreature::returnAttack(CCreature *enemy) {
    int n = _power / 2;
    for (std::vector<CWeapon *>::iterator it = _weaponSet.begin(); it != _weaponSet.end(); it++)
        if ((*it)->_type == 0) {
            n += ((CSword *) *it)->_power;
            (*it)->tryUse(enemy, 1);
            break;
        }
    return n;
}

void CCreature::returnFight(CCreature *enemy) {
    if (_type == 1)return;
    enemy->hurt(returnAttack(enemy));
    if (!_isVirtual) {
        timePrint();
        std::cout << *this << " fought back against " << *enemy << " in city " << _position - city << std::endl;
    }
    if (enemy->isDead() && !_isVirtual) {
        timePrint();
        std::cout << *enemy << " was killed in city " << _position - city << std::endl;
    } else if (_type == 3)((CLion *) this)->_loyalty -= CLion::k;
}

void CCity::raiseFlag(CFaction *faction) {
    if (_lastWin == NULL) {
        _lastWin = faction;
        return;
    }
    if (_lastWin != faction)
        _lastWin = faction;
    else if (_flag != faction) {
        _flag = _lastWin;
        timePrint();
        std::cout << *_lastWin << " flag raised in city " << this - city << std::endl;
    }
}

int main() {
    /*std::ofstream file("rdbuf.txt");
    std::streambuf *x = std::cout.rdbuf(file.rdbuf());*/
    int nCase, iCase = 0;
    std::cin >> nCase;
    while (++iCase <= nCase) {
        std::cout << "Case " << iCase << ':' << std::endl;
        int healthQuota, maxMinute;
        std::cin >> healthQuota >> nCity >> CArrow::_power >> CLion::k >> maxMinute;

        city = new CCity[nCity + 3];
        CFaction redFaction("red", healthQuota);
        CFaction blueFaction("blue", healthQuota);
        redFaction._enemy = &blueFaction;
        blueFaction._enemy = &redFaction;
        std::cin >> CDragon::_initHealth >> CNinja::_initHealth >> CIceman::_initHealth >> CLion::_initHealth
                 >> CWolf::_initHealth;
        std::cin >> CDragon::_initPower >> CNinja::_initPower >> CIceman::_initPower >> CLion::_initPower
                 >> CWolf::_initPower;
        hour = 0;
        while (1) {
            minute = 0;
            if (minute + 60 * hour > maxMinute)break;
            CCreature *redNewWarrior = redFaction.generateWarrior();
            if (redNewWarrior != NULL)redFaction._HQ->_redWarrior = redNewWarrior;
            CCreature *blueNewWarrior = blueFaction.generateWarrior();
            if (blueNewWarrior != NULL)blueFaction._HQ->_blueWarrior = blueNewWarrior;

            minute = 5;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 0; i <= nCity + 1; i++) {
                if (city[i]._redWarrior != NULL)
                    if (city[i]._redWarrior->_type == 3)
                        ((CLion *) (city[i]._redWarrior))->tryFlee();
                if (city[i]._blueWarrior != NULL)
                    if (city[i]._blueWarrior->_type == 3)
                        ((CLion *) (city[i]._blueWarrior))->tryFlee();
            }

            minute = 10;
            if (minute + 60 * hour > maxMinute)break;
            bool redWin = false, blueWin = false;
            for (int i = 0; i <= nCity + 1; i++) {
                if (i >= 1)
                    if (city[i - 1]._redWarrior != NULL) {
                        if (city[i - 1]._redWarrior->_type == 2)((CIceman *) city[i - 1]._redWarrior)->tryMelt();
                        city[i - 1]._redWarrior->displayMarch();
                    }
                if (i <= nCity)
                    if (city[i + 1]._blueWarrior != NULL) {
                        if (city[i + 1]._blueWarrior->_type == 2)((CIceman *) city[i + 1]._blueWarrior)->tryMelt();
                        city[i + 1]._blueWarrior->displayMarch();
                    }
            }
            for (int i = nCity; i >= 0; i--)
                if (city[i]._redWarrior != NULL) {
                    if (city[i + 1]._redWarrior != NULL) {
                        if (i == nCity) {
                            if (city[i + 1]._redWarrior != NULL)delete city[i + 1]._redWarrior;
                            redWin = true;
                        } else std::cout << "Dispalcement Error" << std::endl;
                    }
                    city[i]._redWarrior->_position = &city[i + 1];
                    city[i + 1]._redWarrior = city[i]._redWarrior;
                    city[i]._redWarrior = NULL;
                }
            for (int i = 1; i <= nCity + 1; i++)
                if (city[i]._blueWarrior != NULL) {
                    {
                        if (city[i - 1]._blueWarrior != NULL)
                            if (i == 1) {
                                if (city[i - 1]._blueWarrior != NULL)delete city[i - 1]._blueWarrior;
                                blueWin = true;
                            } else std::cout << "Dispalcement Error" << std::endl;
                    }
                    city[i]._blueWarrior->_position = &city[i - 1];
                    city[i - 1]._blueWarrior = city[i]._blueWarrior;
                    city[i]._blueWarrior = NULL;
                }
            if (redWin) {
                break;
            }
            if (blueWin) {
                break;
            }

            minute = 20;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 1; i <= nCity; i++)
                city[i]._healthBonus += 10;

            minute = 30;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 0; i <= nCity + 1; i++) {
                if (city[i]._blueWarrior == NULL && city[i]._redWarrior != NULL && city[i]._healthBonus != 0) {
                    redFaction._quota += city[i]._healthBonus;
                    timePrint();
                    std::cout << *city[i]._redWarrior << " earned " << city[i]._healthBonus
                              << " elements for his headquarter" << std::endl;
                    city[i]._healthBonus = 0;
                }
                if (city[i]._redWarrior == NULL && city[i]._blueWarrior != NULL && city[i]._healthBonus != 0) {
                    blueFaction._quota += city[i]._healthBonus;
                    timePrint();
                    std::cout << *city[i]._blueWarrior << " earned " << city[i]._healthBonus
                              << " elements for his headquarter" << std::endl;
                    city[i]._healthBonus = 0;
                }
            }

            minute = 35;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 0; i <= nCity + 1; i++) {
                if (city[i]._redWarrior != NULL) {
                    if (&city[i] != blueFaction._HQ && city[i + 1]._blueWarrior != NULL)
                        city[i]._redWarrior->tryArrow(city[i + 1]._blueWarrior);
                }

                if (city[i]._blueWarrior != NULL) {
                    if (&city[i] != redFaction._HQ && city[i - 1]._redWarrior != NULL)

                        city[i]._blueWarrior->tryArrow(city[i - 1]._redWarrior);

                }
            }

            minute = 38;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 0; i <= nCity + 1; i++) {
                if (city[i]._redWarrior != NULL & city[i]._blueWarrior != NULL) {
                    if (!city[i]._redWarrior->isDead() && !city[i]._blueWarrior->isDead()) {
                        if (city[i]._flag == &redFaction)
                            city[i]._redWarrior->tryBomb(city[i]._blueWarrior);

                        if (city[i]._flag == &blueFaction)
                            city[i]._blueWarrior->tryBomb(city[i]._redWarrior);

                        if (city[i]._flag == NULL)
                            if (i % 2)
                                city[i]._redWarrior->tryBomb(city[i]._blueWarrior);
                            else
                                city[i]._blueWarrior->tryBomb(city[i]._redWarrior);
                    }
                }
            }
            minute = 40;
            if (minute + 60 * hour > maxMinute)break;
            std::vector<int> redReward, blueReward;
            for (int i = 0; i <= nCity + 1; i++) {
                if (city[i]._redWarrior != NULL & city[i]._blueWarrior != NULL) {
                    int redHealth = city[i]._redWarrior->_health;
                    int blueHealth = city[i]._blueWarrior->_health;
                    if (!city[i]._redWarrior->isDead() && !city[i]._blueWarrior->isDead()) {
                        if (city[i]._flag == &redFaction)
                            city[i]._redWarrior->fight(city[i]._blueWarrior);

                        if (city[i]._flag == &blueFaction)
                            city[i]._blueWarrior->fight(city[i]._redWarrior);

                        if (city[i]._flag == NULL)
                            if (i % 2)
                                city[i]._redWarrior->fight(city[i]._blueWarrior);
                            else
                                city[i]._blueWarrior->fight(city[i]._redWarrior);
                    }


                    if (!city[i]._redWarrior->isDead() && city[i]._blueWarrior->isDead()) {
                        if (city[i]._redWarrior->_type == 4)
                            ((CWolf *) city[i]._redWarrior)->claimWeapon(city[i]._blueWarrior);
                        if (city[i]._redWarrior->_type == 0)
                            if (city[i]._flag == &redFaction || (city[i]._flag == NULL && i % 2))
                                ((CDragon *) city[i]._redWarrior)->tryCheer(1);
                        redFaction._quota += city[i]._healthBonus;
                        timePrint();
                        std::cout << *city[i]._redWarrior << " earned " << city[i]._healthBonus
                                  << " elements for his headquarter" << std::endl;
                        city[i]._healthBonus = 0;
                        city[i].raiseFlag(&redFaction);
                        redReward.push_back(i);

                        if (city[i]._blueWarrior->_type == 3)
                            city[i]._redWarrior->_health += blueHealth >= 0 ? blueHealth : 0;
                    }

                    if (!city[i]._blueWarrior->isDead() && city[i]._redWarrior->isDead()) {

                        if (city[i]._blueWarrior->_type == 4)
                            ((CWolf *) city[i]._blueWarrior)->claimWeapon(city[i]._redWarrior);
                        if (city[i]._blueWarrior->_type == 0)
                            if (city[i]._flag == &blueFaction || (city[i]._flag == NULL && !(i % 2)))
                                ((CDragon *) city[i]._blueWarrior)->tryCheer(1);

                        blueFaction._quota += city[i]._healthBonus;
                        timePrint();
                        std::cout << *city[i]._blueWarrior << " earned " << city[i]._healthBonus
                                  << " elements for his headquarter" << std::endl;
                        city[i]._healthBonus = 0;
                        city[i].raiseFlag(&blueFaction);
                        blueReward.push_back(i);
                        if (city[i]._redWarrior->_type == 3)
                            city[i]._blueWarrior->_health += redHealth >= 0 ? redHealth : 0;
                    }
                    if (!city[i]._blueWarrior->isDead() && !city[i]._redWarrior->isDead()) {
                        city[i].raiseFlag(NULL);
                        if (city[i]._redWarrior->_type == 0)
                            if (city[i]._flag == &redFaction || (city[i]._flag == NULL && i % 2))
                                ((CDragon *) city[i]._redWarrior)->tryCheer(0);
                        if (city[i]._blueWarrior->_type == 0)
                            if (city[i]._flag == &blueFaction || (city[i]._flag == NULL && !(i % 2)))
                                ((CDragon *) city[i]._blueWarrior)->tryCheer(0);
                    }
                }
                if (city[i]._redWarrior != NULL)
                    if (city[i]._redWarrior->isDead())
                        delete city[i]._redWarrior;

                if (city[i]._blueWarrior != NULL)
                    if (city[i]._blueWarrior->isDead())
                        delete city[i]._blueWarrior;
            }


            std::sort(redReward.begin(), redReward.end(), std::less<int>());
            std::sort(blueReward.begin(), blueReward.end(), std::greater<int>());
            for (std::vector<int>::iterator it = redReward.begin(); it != redReward.end(); it++)
                if (redFaction._quota >= 8) {
                    redFaction._quota -= 8;
                    city[*it]._redWarrior->_health += 8;
                }
            for (std::vector<int>::iterator it = blueReward.begin(); it != blueReward.end(); it++)
                if (blueFaction._quota >= 8) {
                    blueFaction._quota -= 8;
                    city[*it]._blueWarrior->_health += 8;
                }

            minute = 50;
            if (minute + 60 * hour > maxMinute)break;
            timePrint();
            std::cout << redFaction._quota << " elements in red headquarter" << std::endl;
            timePrint();
            std::cout << blueFaction._quota << " elements in blue headquarter" << std::endl;

            minute = 55;
            if (minute + 60 * hour > maxMinute)break;
            for (int i = 0; i <= nCity + 1; i++)
                if (city[i]._redWarrior != NULL)city[i]._redWarrior->reportWeapon();

            for (int i = 0; i <= nCity + 1; i++)
                if (city[i]._blueWarrior != NULL)city[i]._blueWarrior->reportWeapon();
            hour++;
        }
        delete[] city;
    }
    getchar();
    getchar();
    return 0;
}