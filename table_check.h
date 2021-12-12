#ifndef TABLE_CHECK_H
#define TABLE_CHECK_H

extern int N;

bool isassociative(int table[N*N]);
bool iscommutative(int table[N*N]);
bool isidempotent(int table[N*N]);
bool isdistributive_left(int mult[N*N], int add[N*N]);
bool isdistributive_right(int mult[N*N], int add[N*N]);
bool isdistributive(int mult[N*N], int add[N*N]);

#endif // !TABLE_CHECK_H
