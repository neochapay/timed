/***************************************************************************
**                                                                        **
**   Copyright (C) 2009-2011 Nokia Corporation.                           **
**                                                                        **
**   Author: Ilya Dogolazky <ilya.dogolazky@nokia.com>                    **
**   Author: Simo Piiroinen <simo.piiroinen@nokia.com>                    **
**   Author: Victor Portnov <ext-victor.portnov@nokia.com>                **
**                                                                        **
**     This file is part of Timed                                         **
**                                                                        **
**     Timed is free software; you can redistribute it and/or modify      **
**     it under the terms of the GNU Lesser General Public License        **
**     version 2.1 as published by the Free Software Foundation.          **
**                                                                        **
**     Timed is distributed in the hope that it will be useful, but       **
**     WITHOUT ANY WARRANTY;  without even the implied warranty  of       **
**     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               **
**     See the GNU Lesser General Public License  for more details.       **
**                                                                        **
**   You should have received a copy of the GNU  Lesser General Public    **
**   License along with Timed. If not, see http://www.gnu.org/licenses/   **
**                                                                        **
***************************************************************************/
#ifndef MAEMO_TIMED_TZ_H
#define MAEMO_TIMED_TZ_H

#include <map>
#include <string>
using namespace std;

#include <QTimer>

#include "cellular.h"

class olson;

struct status_t
{
    status_t()
        : last_zone(NULL)
    {}
    olson *last_zone;
};

enum guess_quality { Uncertain, Reliable, Confirmed, Canceled, Waiting, Initial };

struct tz_suggestions_t // obsolete
{
    vector<olson *> zones;
    guess_quality gq;

    void add(olson *tz) { zones.push_back(tz); }
    void set(vector<olson *> list) { zones = list; }
    tz_suggestions_t() { gq = Uncertain; }
};

struct suggestion_t
{
    std::map<olson *, int> s;
    void add(olson *zone, int score);
};

struct tz_oracle_t : public QObject
{
    static const int nitz_wait_ms = 1000;

    QTimer *timer;

    struct operator_status_t
    {
        operator_status_t() {}
        operator_status_t(const cellular_operator_t &op)
            : oper(op)
        {}
        operator_status_t(const operator_status_t &other)
            : oper(other.oper)
            , stat(other.stat)
        {}
        cellular_operator_t oper;
        status_t stat;
    };
    QMap<QString, operator_status_t> operators; // modemPath key

    tz_oracle_t();
    ~tz_oracle_t();
public slots:
    void waiting_for_nitz_timeout();
    void cellular_operator(const cellular_operator_t &data, const QString &modem);
    void cellular_offset(const cellular_offset_t &data);

signals:
    void cellular_zone_detected(olson *, suggestion_t, bool);

private:
    void set_by_offset(const cellular_offset_t &data);
    // void set_by_operator(const cellular_operator_t &o) ;
    void set_by_operator(const QString &modem);

    void output(olson *zone, suggestion_t *s, bool sure);
    void output(olson *zone);

    Q_OBJECT;
};

#endif
