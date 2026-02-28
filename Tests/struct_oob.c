struct Data {
    int ID;
    char Name[4];
};

void struct_oob() {
    struct Data arr[2]; 
    arr[0].Name[6] = 'a'; 
}
