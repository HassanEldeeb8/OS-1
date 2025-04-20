#include <iostream>
#include <openssl/des.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>

using namespace std;

// Convert binary string to hex string
string toHexString(const string& input) {
    stringstream ss;
    for (unsigned char c : input) {
        ss << hex << setw(2) << setfill('0') << (int)c;
    }
    return ss.str();
}

// Encrypt string with DES 25 times
string desEncrypt25(const string& plain, const string& salt) {
    DES_cblock key;
    DES_key_schedule schedule;

    // Pad salt to 8 bytes for key
    string paddedSalt = salt;
    paddedSalt.resize(8, '0');
    memcpy(key, paddedSalt.c_str(), 8);

    DES_set_key_unchecked(&key, &schedule);

    string data = plain;
    data.resize(8, ' '); // pad to 8 bytes

    DES_cblock inputBlock, outputBlock;
    memcpy(inputBlock, data.c_str(), 8);

    // Repeat DES encryption 25 times
    for (int i = 0; i < 25; ++i) {
        DES_ecb_encrypt(&inputBlock, &outputBlock, &schedule, DES_ENCRYPT);
        memcpy(inputBlock, outputBlock, 8);
    }

    return toHexString(string((char*)outputBlock, 8));
}

// Generate random 2-letter salt
string randomSalt() {
    string salt = "";
    salt += 'A' + rand() % 26;
    salt += 'A' + rand() % 26;
    return salt;
}

int main() {
    srand(time(nullptr));
    vector<string> encryptedPasswords;

    cout << "🔐 Generating encrypted passwords:\n";

    for (int i = 1; i <= 10; ++i) {
        string password = "User" + to_string(i);
        string salt = randomSalt();
        string encrypted = desEncrypt25(password, salt);

        cout << i << ". Password: " << password
             << " | Salt: " << salt
             << " | Encrypted: " << encrypted << endl;

        encryptedPasswords.push_back(encrypted);
    }

    // User input check
    cout << "\n🔎 Enter password to verify: ";
    string userPass, userSalt;
    cin >> userPass;
    cout << "Enter salt: ";
    cin >> userSalt;

    string userEncrypted = desEncrypt25(userPass, userSalt);
    cout << "Encrypted version: " << userEncrypted << endl;

    bool matchFound = false;
    for (const string& stored : encryptedPasswords) {
        if (stored == userEncrypted) {
            matchFound = true;
            break;
        }
    }

    cout << (matchFound ? "✅ Match found!" : "❌ No match.") << endl;
    return 0;
}

