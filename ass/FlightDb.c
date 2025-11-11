
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "List.h"
#include "FlightDb.h"
#include "AVLTree.h"

struct flightDb { // To create three trees which sorted by byFlightNumber, byDepartureAirport and byTimes
	// TODO: Add to this struct
	Tree byFlightNumber;
	Tree byDepartureAirport;
	Tree byTimes;
};

////////////////////////////////////////////////////////////////////////
// Comparison functions
int comparebyTimes(Record r1, Record r2) {
// In comparebyTimes, firstly we compare by Day, if the same day, then compare by hour
// If the same hour, then compare the Minute, if all the time data is equal, try to find a data 
// which combines it with Times can totally comfirm a exactly flight, so i choose the flightnumber
	int cmpDay = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	int cmpHour = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	int cmpMinute = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	int cmpFlightNumber = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2)); // If compare string, using the strcmp()
	if (cmpDay != 0) {
		return cmpDay;
	} if (cmpHour != 0) {
		return cmpHour;
	} if (cmpMinute != 0) {
		return cmpMinute;
	} 
	return cmpFlightNumber;
}

int comparebyFlightNumber(Record r1, Record r2) {
// The same idea with comparebyTimes, if meets the same flightnumber, then the Times can totally
// determine a exactly fligtt(no flight has the same flight number and Times)
	int cmpFlightNumber = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
	int cmpDay = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	int cmpHour = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	int cmpMinute = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	if (cmpFlightNumber != 0) {
		return cmpFlightNumber;
	} if (cmpDay != 0) {
		return cmpDay;
	} if (cmpHour != 0) {
		return cmpHour;
	} 
	return cmpMinute;
}

int comparebyDepartureAirport(Record r1, Record r2) {
// Using the same idea as the Time comparision, only difference is we give the departureAirport
// the priority, then exactly the same as comparebyTime
	int cmpDeparureAirport = strcmp(RecordGetDepartureAirport(r1), RecordGetDepartureAirport(r2));
	int cmpDay = RecordGetDepartureDay(r1) - RecordGetDepartureDay(r2);
	int cmpHour = RecordGetDepartureHour(r1) - RecordGetDepartureHour(r2);
	int cmpMinute = RecordGetDepartureMinute(r1) - RecordGetDepartureMinute(r2);
	int cmpFlightNumber = strcmp(RecordGetFlightNumber(r1), RecordGetFlightNumber(r2));
	if (cmpDeparureAirport != 0) {
		return cmpDeparureAirport;
	} if (cmpDay != 0) {
		return cmpDay;
	} if (cmpHour != 0) {
		return cmpHour;
	} if (cmpMinute != 0) {
		return cmpMinute;
	}
	return cmpFlightNumber;
}
////////////////////////////////////////////////////////////////////////
/**
 * Creates a new flight DB. 
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
FlightDb DbNew(void) {
	// TODO: Complete this function
	FlightDb db = malloc(sizeof(*db));
    if (db == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

	db->byFlightNumber = TreeNew(comparebyFlightNumber);
    db->byDepartureAirport = TreeNew(comparebyDepartureAirport);
	db->byTimes = TreeNew(comparebyTimes);

    return db;
}

/**
 * Frees all memory allocated to the given flight DB
 */
void	DbFree(FlightDb db) {
	// TODO: Complete this function
	TreeFree(db->byFlightNumber, false);
    TreeFree(db->byDepartureAirport, false);
	TreeFree(db->byTimes, true);
    free(db);
}

/**
 * Inserts  a  flight  record  into the given DB if there is not already
 * record with the same flight number, departure airport, day, hour  and
 * minute.
 * If  inserted successfully, this function takes ownership of the given 
 * record (so the caller should not modify or free it). 
 * Returns true if the record was successfully inserted,  and  false  if
 * the  DB  already  contained  a  record  with  the same flight number,
 * departure airport, day, hour and minute.
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
bool     DbInsertRecord(FlightDb db, Record r) {
	// TODO: Complete this function
	if (TreeInsert(db->byFlightNumber, r)) {
		TreeInsert(db->byDepartureAirport, r);
		TreeInsert(db->byTimes, r);
		return true;
	}
	return false;
}

/**
 * Searches  for  all  records with the given flight number, and returns
 * them all in a list in increasing order of  (day, hour, min).  Returns
 * an empty list if there are no such records. 
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByFlightNumber(FlightDb db, char *flightNumber) {
	// TODO: Complete this function
// Find a list of fligths with the given flightnumber, so by the comparebyFlightNumber()
// if with the same flightnumebr, we can distinguish by Times, so create two dummy records which are the lowest and uppest records
// among all the sameflightnumber record, (lower is the first moment of the week, upper is the last moment of the week
// then put them into searchbetween function can get the ordered list
	Record dummylower = RecordNew(flightNumber, "", "", 0, 0, 0, 0);
	Record dummyupper = RecordNew(flightNumber, "", "", 6, 23, 59, 200);
	List rec = TreeSearchBetween(db->byFlightNumber, dummylower, dummyupper);
	RecordFree(dummylower); // Remember to free these two dummy records to aviod any memory leaks in my program
	RecordFree(dummyupper);
	return rec;
}

/**
 * Searches  for all records with the given departure airport and day of
 * week (0 to 6), and returns them all in a list in increasing order  of
 * (hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindByDepartureAirportDay(FlightDb db, char *departureAirport,
                                     int day) {
	// TODO: Complete this function
// Same idea with the DbFindByFlightNumber(), but the difference is i notice that by the departureAirport
// and the Times can exactly confirm a certain flight, as the same departureAirport, lower dummy record is 
// the first moment of the week , upper dummy record is the last moment of the week
	Record dummylower = RecordNew("", departureAirport, "", day, 0, 0, 0);
	Record dummyupper = RecordNew("", departureAirport, "", day, 23, 59, 200);
	List rec = TreeSearchBetween(db->byDepartureAirport, dummylower, dummyupper);
	RecordFree(dummylower); // Remember to free these two dummy records to aviod any memory leaks in my program
	RecordFree(dummyupper);
	return rec;
}


/**
 * Searches  for  all  records  between  (day1, hour1, min1)  and (day2,
 * hour2, min2), and returns them all in a list in increasing  order  of
 * (day, hour, min, flight number).
 * Returns an empty list if there are no such records.
 * The  records  in the returned list should not be freed, but it is the
 * caller's responsibility to free the list itself.
 * The time complexity of this function must be O(log n + m), where m is
 * the length of the returned list.
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
List     DbFindBetweenTimes(FlightDb db, 
                            int day1, int hour1, int min1, 
                            int day2, int hour2, int min2) {
	// TODO: Complete this function
// Given a list of records beteeen two times, just make sure dummylower will be the smallest record among the same Times
// records (can achieve by using smallest flight number which i consider is just ""), dummyupper will be the biggest among the 
// same Times records (can achieve by using biggest flight number, by looking at ASCII table, and the constrain size is 8, so i just take "zzzzzzzz")
	Record dummylower = RecordNew("", "", "", day1, hour1, min1, 0);
	Record dummyupper = RecordNew("zzzzzzzz", "", "", day2, hour2, min2, 200);
	List rec = TreeSearchBetween(db->byTimes, dummylower, dummyupper);
	RecordFree(dummylower);
	RecordFree(dummyupper); // Remember to free these two dummy records to aviod any memory leaks in my program
	return rec;
	
}

/**
 * Searches  for  and  returns  the  earliest next flight from the given
 * departure airport, on or after the given (day, hour, min), or NULL if
 * there is no such flight.
 * The returned record must not be freed or modified. 
 * The time complexity of this function must be O(log n).
 * You MUST use the AVLTree ADT (from Task 1) in your implementation.
 */
Record   DbFindNextFlight(FlightDb db, char *departureAirport, 
                          int day, int hour, int min) {
	// TODO: Complete this function
//
	Record dummy = RecordNew("", departureAirport, "", day, hour, min, 200);
	Record rec = TreeNext(db->byDepartureAirport, dummy);
// If the given record is the last flight of this departureAirport this week, then the next should be the first flight
// of next week in this departureAirport, so take the given epartureAirport, make a smallest dummy record of the week
// which is the first moment of the week
	if (rec == NULL) {
		Record flightnextweek = RecordNew("", departureAirport, "", 0, 0, 0, 200);
		rec = TreeNext(db->byDepartureAirport, flightnextweek);
		RecordFree(flightnextweek);
	}
	RecordFree(dummy); // Remember to free these two dummy records to aviod any memory leaks in my program
	return rec;
}

