#include "passserver.h"

// Xian: in a shell, type "man crypt", and the MD5 can be found (which uses 22 characters of the password), and the usage will be shown as well.
// Note that it takes c_str, char [], not string directly as the input arguments.

PassServer::PassServer(size_t size) : h(size) {} // in private HashTable<string,string> h;

PassServer::~PassServer()
{
    h.clear();
}

bool PassServer::load(const char *filename)
{
    return h.load(filename);
}
bool PassServer::addUser(std::pair<string, string> &kv)
{
    string encrypted_password = encrypt(kv.second);
    if (h.contains(kv.first))
    {
        cout << "user already exists" << endl;
        return false; // Username already exists, don't add
    }
    if (!h.contains(kv.first))
    {
        h.insert(std::make_pair(kv.first, encrypted_password));
        return true;
    }

    return false;
}
bool PassServer::addUser(std::pair<string, string> &&kv)
{
    string encrypted_password = encrypt(kv.second);
    if (h.contains(kv.first))
    {
        cout << "user already exists" << endl;
        return false; // Username already exists, don't add
    }
    if (!h.contains(kv.first))
    {
        h.insert(std::make_pair(std::move(kv.first), std::move(encrypted_password)));
        return true;
    }
    return false;
}
bool PassServer::removeUser(const string &k)
{
    return h.remove(k);
}
bool PassServer::changePassword(const std::pair<string, string> &p, const string &newpassword)
{
    if (!h.contains(p.first))
    {
        std::cerr << "Error: User not found" << endl;
        return false;
    }
    std::string encryptedOldPassword = encrypt(p.second);
    if (!h.match(std::make_pair(p.first, encryptedOldPassword)))
    {
        std::cerr << "Error: Old password does not match." << endl;
        return false;
    }
    std::string encryptedNewPassword = encrypt(newpassword);
    if (encryptedNewPassword == encryptedOldPassword)
    {
        std::cerr << "Error: New password must be different from old password" << endl;
        return false;
    }
    if (!h.insert(std::make_pair(p.first, encryptedNewPassword)))
    {
        std::cerr << "Error: Could not update password" << endl;
        return false;
    }
    return true;
}
bool PassServer::find(const string &user) const
{
    return h.contains(user);
}
void PassServer::dump() const
{
    return h.dump();
}
size_t PassServer::size() const
{
    return h.size();
}
bool PassServer::write_to_file(const char *filename) const
{
    return h.write_to_file(filename);
}
string PassServer::encrypt(const string &str)
{
    char salt[] = "$1$########";
    char *password = new char[35];
    strcpy(password, crypt(str.c_str(), salt));

    string pword(password);
    string delim = "$";
    int pos = pword.find(delim, 0);
    pos = pword.find(delim, pos + 1);
    pos = pword.find(delim, pos + 1);
    pword.erase(0, pos + 1);
    return pword;
    /*char salt[] = "$1$########";
    string ret = crypt(str.c_str(), salt);
    return ret.substr(1, 22);*/
};