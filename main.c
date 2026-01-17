#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h> 
#include <math.h>

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

// ================== ENUMS ========================
// Customer types
//Enum numerical value reflects the priority given for each customer type
//(lower value = higher priority)
enum CustomerType {
    DISABLED = 0,
    VIP = 1,
    STAFF = 2,
    REGISTERED = 3,
    GUEST = 4,
    CUSTOMER_TYPE_COUNT
};

static const char* CustomerTypeNames[] = {
    "DISABLED",
    "VIP",
    "STAFF",
    "REGISTERED",
    "GUEST"
};

static const char* CustomerTypeSymbols[] = {
    "D",
    "V",
    "S",
    "R",
    "G"
};

// Compile-time assertion to ensure CustomerType array is in sync
_Static_assert(CUSTOMER_TYPE_COUNT == ARRAY_COUNT(CustomerTypeNames),
               "CustomerTypeNames array is out of sync!");
               
// Compile-time assertion to ensure CustomerTypeSymbols array is in sync
_Static_assert(CUSTOMER_TYPE_COUNT == ARRAY_COUNT(CustomerTypeSymbols),
               "CustomerTypeSymbols array is out of sync!");

// Vehicle types
enum VehicleType {
    MOTORCYCLE = 0,
    THREE_WHEELER = 1,
    CAR = 2,
    VAN = 3,
    BUS = 4,
    VEHICLE_TYPE_COUNT
};
static const char* VehicleTypeNames[] = {
    "MOTORCYCLE",
    "THREE WHEELER",
    "CAR",
    "VAN",
    "BUS"
};
// Compile-time assertion to ensure VehicleTypeNames array is in sync
_Static_assert(VEHICLE_TYPE_COUNT == ARRAY_COUNT(VehicleTypeNames),
               "VehicleTypeNames array is out of sync!");
// ================================================

// =====================CONSTANTS =================
//Vehicle capacities
const int MAX_MOTORCYCLES = 100;
const int MAX_THREE_WHEELERS = 75;
const int MAX_CARS = 100;
const int MAX_VANS = 50;
const int MAX_BUSES = 30;
#define TOTAL_SLOTS 355

// Fee structure
const int FIRST_HOUR_FEE[] = {
    [MOTORCYCLE] = 20,
    [THREE_WHEELER] = 30,
    [CAR] = 40,
    [VAN] = 50,
    [BUS] = 80
};

const int ADDITIONAL_HOUR_FEE[] = {
    [MOTORCYCLE] = 30,
    [THREE_WHEELER] = 40,
    [CAR] = 50,
    [VAN] = 60,
    [BUS] = 100
};

const float DISCOUNTS[] = {
    [DISABLED] = 0.6,
    [VIP] = 0.5,
    [STAFF] = 0.8,
    [REGISTERED] = 0.3,
    [GUEST] = 0.0
};
// ================================================

// ================== STRUCTS =====================
// Parking slot structure
typedef struct {
    int total;
    int allocated;
    int free;
} ParkingSlot;

// Vehicle structure
typedef struct {
    char vehicleNumber[20];
    enum VehicleType vehicleType;
    enum CustomerType customerType;
    time_t arrivalTime;
} Vehicle;

// Parking management structure
typedef struct {
    ParkingSlot slots[5][5]; // [vehicleType][customerType]
    Vehicle vehicles[TOTAL_SLOTS];
    int vehicleCount;
} ParkingManagement;
// ================================================


ParkingManagement parking;

// Function prototypes
void initializeParking();
void loadParkingData();
void saveParkingData();
void displayMenu();
void checkAvailability();
void enterVehicle();
void exitVehicle();
void viewParkingSpace();
void viewStatistics();
void viewStatisticsTableView();
void viewStatisticsGraph();
void generateBill(Vehicle vehicle, time_t exitTime);

int main() {
    initializeParking();
    loadParkingData();

    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                checkAvailability();
                break;
            case 2:
                enterVehicle();
                break;
            case 3:
                exitVehicle();
                break;
            case 4:
                viewParkingSpace();
                break;
            case 5:
                viewStatistics();
                break;
            case 6:
                saveParkingData();
                printf("Exiting the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}

void displayMenu() {
    printf("\n+============= Main Menu =============+\n");
    printf("| %-2s | %-30s |\n", "No", "Option");
    printf("+-------------------------------------+\n");
    printf("| %-2d | %-30s |\n", 1, "Check Availability");
    printf("| %-2d | %-30s |\n", 2, "Enter Vehicle");
    printf("| %-2d | %-30s |\n", 3, "Exit Vehicle");
    printf("| %-2d | %-30s |\n", 4, "View Parking Space");
    printf("| %-2d | %-30s |\n", 5, "View Statistics");
    printf("| %-2d | %-30s |\n", 6, "Exit");
    printf("+=====================================+\n");
}

void initializeParking() {
    int capacities[] = {MAX_MOTORCYCLES, MAX_THREE_WHEELERS, MAX_CARS, MAX_VANS, MAX_BUSES};
    float percentages[] = {0.15, 0.10, 0.15, 0.25, 0.35};

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            parking.slots[i][j].total = (int)(capacities[i] * percentages[j]);
            parking.slots[i][j].allocated = 0;
            parking.slots[i][j].free = parking.slots[i][j].total;
        }
    }
    parking.vehicleCount = 0;
}

void loadParkingData() {
    FILE *file = fopen("parking_data.txt", "r");
    if (!file) return;

    fscanf(file, "%d\n", &parking.vehicleCount);

    for (int i = 0; i < parking.vehicleCount; i++) {
        Vehicle v;
        fscanf(file, "%s %d %d %ld\n",
               v.vehicleNumber,
               &v.vehicleType,
               &v.customerType,
               &v.arrivalTime);

        parking.vehicles[i] = v;
        parking.slots[v.vehicleType][v.customerType].allocated++;
        parking.slots[v.vehicleType][v.customerType].free--;
    }
    fclose(file);
}

void saveParkingData() {
    FILE *file = fopen("parking_data.txt", "w");
    if (!file) return;

    fprintf(file, "%d\n", parking.vehicleCount);

    for (int i = 0; i < parking.vehicleCount; i++) {
        Vehicle v = parking.vehicles[i];
        fprintf(file, "%s %d %d %ld\n",
                v.vehicleNumber,
                v.vehicleType,
                v.customerType,
                v.arrivalTime);
    }
    fclose(file);
}

// void enterVehicle() {
//     if (parking.vehicleCount >= TOTAL_SLOTS) {
//         printf("\n+==============================================+\n");
//         printf("| Parking is full. Cannot accept more vehicles. |\n");
//         printf("+==============================================+\n\n");
//         return;
//     }

//     Vehicle vehicle;

//     printf("\n+=================== Enter Vehicle ===================+\n");

//     // Vehicle number input
//     printf("| %-40s: ", "Vehicle Number");
//     scanf("%s", vehicle.vehicleNumber);

//     // Vehicle type input
//     printf("\n| %-40s: \n", "Vehicle Type");
//     for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
//         printf("|   (%d) %-33s |\n", i, VehicleTypeNames[i]);
//     }
//     printf("| %-40s: ", "Enter Choice");
//     scanf("%d", &vehicle.vehicleType);

//     // Customer type input
//     printf("\n| %-40s: \n", "Customer Type");
//     for (int i = 0; i < CUSTOMER_TYPE_COUNT; i++) {
//         printf("|   (%d) %-33s |\n", i, CustomerTypeNames[i]);
//     }
//     printf("| %-40s: ", "Enter Choice");
//     scanf("%d", &vehicle.customerType);

//     ParkingSlot *slot = &parking.slots[vehicle.vehicleType][vehicle.customerType];

//     // Success / failure message
//     if (slot->free > 0) {
//         slot->allocated++;
//         slot->free--;
//         vehicle.arrivalTime = time(NULL);
//         parking.vehicles[parking.vehicleCount++] = vehicle;

//         printf("+====================================================+\n");
//         printf("| Vehicle parked successfully!                       |\n");
//         printf("+====================================================+\n\n");
//     } else {
//         printf("+====================================================+\n");
//         printf("| No free slots available for this vehicle and      |\n");
//         printf("| customer type.                                     |\n");
//         printf("+====================================================+\n\n");
//     }
// }

void enterVehicle() {
    if (parking.vehicleCount >= TOTAL_SLOTS) {
        printf("\n+==============================================+\n");
        printf("| Parking is full. Cannot accept more vehicles. |\n");
        printf("+==============================================+\n\n");
        return;
    }

    Vehicle vehicle;

    printf("\n+=================== Enter Vehicle ===================+\n");

    // Vehicle number input
    printf("| %-40s: ", "Vehicle Number");
    scanf("%s", vehicle.vehicleNumber);

    // Vehicle type input
    printf("\n| %-40s: \n", "Vehicle Type");
    for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
        printf("|   (%d) %-33s |\n", i, VehicleTypeNames[i]);
    }
    printf("| %-40s: ", "Enter Choice");
    scanf("%d", &vehicle.vehicleType);

    // Customer type input
    printf("\n| %-40s: \n", "Customer Type");
    for (int i = 0; i < CUSTOMER_TYPE_COUNT; i++) {
        printf("|   (%d) %-33s |\n", i, CustomerTypeNames[i]);
    }
    printf("| %-40s: ", "Enter Choice");
    scanf("%d", &vehicle.customerType);

    ParkingSlot *allocatedSlot = NULL;
    enum CustomerType allocatedCustomerType = vehicle.customerType;

    ParkingSlot *ownSlot = &parking.slots[vehicle.vehicleType][vehicle.customerType];
    if (ownSlot->free > 0) {
        allocatedSlot = ownSlot;
    } 
    else {
        for (enum CustomerType c = DISABLED; c < vehicle.customerType; c++) {
            ParkingSlot *candidate = &parking.slots[vehicle.vehicleType][c];

            if (candidate->free <= 0)
                continue;

            float freeRatio = (float)candidate->free / candidate->total;

            // Disabled slots rule
            if (c == DISABLED && freeRatio <= 0.60)
                continue;

            // VIP slots rule
            if (c == VIP && freeRatio <= 0.50)
                continue;

            allocatedSlot = candidate;
            allocatedCustomerType = c;
            break;
        }
    }

    // Final allocation decision
    if (allocatedSlot) {
        allocatedSlot->allocated++;
        allocatedSlot->free--;

        vehicle.customerType = allocatedCustomerType;
        vehicle.arrivalTime = time(NULL);
        parking.vehicles[parking.vehicleCount++] = vehicle;

        printf("\n+====================================================+\n");
        printf("| Vehicle parked successfully under %-12s slot |\n",
               CustomerTypeNames[allocatedCustomerType]);
        printf("+====================================================+\n\n");
    } else {
        printf("\n+====================================================+\n");
        printf("| No suitable slot available based on priority rules |\n");
        printf("+====================================================+\n\n");
    }
}

void checkAvailability() {
    int backChoice;

    do {
        printf("\n================ Availability ================\n");

        printf("%-15s | %-15s | %5s\n", "Vehicle Type", "Customer Type", "Free");
        printf("--------------------------------------------\n");

        for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
            for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
                printf("%-15s | %-15s | %5d\n",
                       VehicleTypeNames[i],        // Vehicle Type
                       CustomerTypeNames[j],       // Customer Type
                       parking.slots[i][j].free   // Free count
                );
            }
            printf("\n");
        }

        printf("============================================\n");

        printf("\nEnter 0 to go back to the Main Menu: ");
        scanf("%d", &backChoice);
        getchar();
    } while (backChoice != 0);
}

void exitVehicle() {
    char vehicleNumber[20];

    printf("\n==================== Exit Vehicle ===================\n");
    printf("| %-30s: ", "Enter vehicle number");
    scanf("%s", vehicleNumber);

    for (int i = 0; i < parking.vehicleCount; i++) {
        if (strcmp(parking.vehicles[i].vehicleNumber, vehicleNumber) == 0) {
            Vehicle vehicle = parking.vehicles[i];
            time_t exitTime = time(NULL);

            ParkingSlot *slot = &parking.slots[vehicle.vehicleType][vehicle.customerType];
            slot->allocated--;
            slot->free++;

            generateBill(vehicle, exitTime);

            for (int j = i; j < parking.vehicleCount - 1; j++) {
                parking.vehicles[j] = parking.vehicles[j + 1]; 
            }
            parking.vehicleCount--;

            printf("\n+--------------------------------------------+\n");
            printf("| Vehicle successfully exited the parking!  |\n");
            printf("+--------------------------------------------+\n\n");

            return;
        }
    }

    printf("\n+--------------------------------------------+\n");
    printf("| Vehicle not found in the parking system.   |\n");
    printf("+--------------------------------------------+\n\n");
}

// void generateBill(Vehicle vehicle, time_t exitTime) {
//     // Calculate parking duration
//     double duration = difftime(exitTime, vehicle.arrivalTime) / 3600.0; // in hours
//     int hours = (int)duration;
//     int minutes = (int)((duration - hours) * 60);

//     // Calculate fees
//     int fee = FIRST_HOUR_FEE[vehicle.vehicleType];
//     if (hours > 1) {
//         fee += (hours - 1) * ADDITIONAL_HOUR_FEE[vehicle.vehicleType];
//     }

//     float discount = fee * DISCOUNTS[vehicle.customerType];
//     float totalPayable = fee - discount;

//     // Print the parking bill
//     printf("\n+=================== Parking Bill ===================+\n");
//     printf("| %-20s : %s\n", "Vehicle Number", vehicle.vehicleNumber);
//     printf("| %-20s : %s\n", "Vehicle Type", VehicleTypeNames[vehicle.vehicleType]);
//     printf("| %-20s : %s\n", "Customer Type", CustomerTypeNames[vehicle.customerType]);
//     printf("| %-20s : %s", "Entered Time", ctime(&vehicle.arrivalTime));
//     printf("| %-20s : %s", "Exit Time", ctime(&exitTime));
//     printf("| %-20s : %d hours %d minutes\n", "Parking Duration", hours, minutes);
//     printf("| %-20s : Rs %d.00\n", "Total Charge", fee);
//     printf("| %-20s : Rs %.2f\n", "Discount", discount);
//     printf("| %-20s : Rs %.2f\n", "Total Payable", totalPayable);
//     printf("+====================================================+\n\n");
// }

void generateBill(Vehicle vehicle, time_t exitTime) {
    // Calculate exact duration in seconds
    double durationSeconds = difftime(exitTime, vehicle.arrivalTime);

    // Display duration
    int totalMinutes = (int)(durationSeconds / 60);
    int displayHours = totalMinutes / 60;
    int displayMinutes = totalMinutes % 60;

    // Billing hours (round UP)
    double durationHours = durationSeconds / 3600.0;
    int billableHours = (int)ceil(durationHours);
    if (billableHours < 1)
        billableHours = 1;

    // Calculate fees
    int fee = FIRST_HOUR_FEE[vehicle.vehicleType];
    if (billableHours > 1) {
        fee += (billableHours - 1) * ADDITIONAL_HOUR_FEE[vehicle.vehicleType];
    }

    // Apply discount
    float discount = fee * DISCOUNTS[vehicle.customerType];
    float totalPayable = fee - discount;

    // Print the parking bill
    printf("\n+=================== Parking Bill ===================+\n");
    printf("| %-20s : %s\n", "Vehicle Number", vehicle.vehicleNumber);
    printf("| %-20s : %s\n", "Vehicle Type", VehicleTypeNames[vehicle.vehicleType]);
    printf("| %-20s : %s\n", "Customer Type", CustomerTypeNames[vehicle.customerType]);
    printf("| %-20s : %s", "Entered Time", ctime(&vehicle.arrivalTime));
    printf("| %-20s : %s", "Exit Time", ctime(&exitTime));
    printf("| %-20s : %d hours %d minutes\n",
           "Parking Duration", displayHours, displayMinutes);
    printf("| %-20s : Rs %d.00\n", "Total Charge", fee);
    printf("| %-20s : Rs %.2f\n", "Discount", discount);
    printf("| %-20s : Rs %.2f\n", "Total Payable", totalPayable);
    printf("+====================================================+\n\n");
}

void viewParkingSpace() {
    printf("\n+======================================================= Parking Space ============================================================\n\n");

    for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
        printf("%-15s: { ", VehicleTypeNames[i]);

        for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
            int allocated = parking.slots[i][j].allocated;
            int free = parking.slots[i][j].free;

            for (int k = 0; k < allocated; k++) {
                printf("%s", CustomerTypeSymbols[j]);
            }

            for (int k = 0; k < free; k++) {
                printf("F");
            }

            if (j < CUSTOMER_TYPE_COUNT - 1) {
                printf(" | ");
            }
        }

        printf(" }\n");
    }

    printf("\n+================================================================================================================================+\n\n");
}

void viewStatistics() {
    int choice = 1; 

    do {
        if (choice == 1) {
            viewStatisticsTableView();
        } else if (choice == 2) {
            viewStatisticsGraph();
        }

        printf("\n| %-18s | %-18s | %-18s \n", "1: Table View", "2: Graph View", "0: Go Back");
        printf("\n");
        printf("+=============================================================+\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        if (choice != 0 && choice != 1 && choice != 2) {
            printf("Invalid choice. Please try again.\n");
            choice = 1;
        }

    } while (choice != 0);
}

void viewStatisticsTableView() {
    printf("\n+=================== Parking Statistics ======================+\n");

    printf("| %-15s | %-15s | %-10s | %-10s |\n", 
           "Vehicle Type", "Customer Type", "Allocated", "Free");
    printf("+-------------------------------------------------------------+\n");

    for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
        for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
            printf("| %-15s | %-15s | %-10d | %-10d |\n",
                   VehicleTypeNames[i],
                   CustomerTypeNames[j],
                   parking.slots[i][j].allocated,
                   parking.slots[i][j].free);
        }
        printf("+-------------------------------------------------------------+\n"); 
    }

    printf("+=============================================================+\n");
}

void viewStatisticsGraph() {
    int vehicleColWidth = 15;  
    int customerColWidth = 25;  
    int barWidth = 20;

    printf("\n+==================================================================== Parking Graph ===================================================================+\n");

    printf("| %-*s|", vehicleColWidth, "Vehicle Type");
    for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
        printf(" %-*s ", customerColWidth, CustomerTypeNames[j]);
    }
    printf("\n");

    printf("");
    printf("%.*s", vehicleColWidth + 2, "==============================");
    for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
        printf("%.*s", customerColWidth + 2, "===============================");
    }
    printf("\n");

    for (int i = 0; i < VEHICLE_TYPE_COUNT; i++) {
        printf("|%-*s |", vehicleColWidth, VehicleTypeNames[i]);
        for (int j = 0; j < CUSTOMER_TYPE_COUNT; j++) {
            int allocated = parking.slots[i][j].allocated;
            int free = parking.slots[i][j].free;
            int totalSlots = allocated + free;

            int scale = totalSlots > barWidth ? totalSlots / barWidth + 1 : 1;
            int allocBars = allocated / scale;
            int freeBars = free / scale;

            for (int k = 0; k < allocBars; k++) printf("#");
            for (int k = 0; k < freeBars; k++) printf("*");

            int remaining = customerColWidth - (allocBars + freeBars);
            for (int k = 0; k < remaining; k++) printf(" ");
        }
        printf("\n");
    }

    printf("+===================================================================================================================================================+\n");
}
