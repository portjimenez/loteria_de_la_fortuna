#include <windows.h>
#include <iostream>
#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysql.h"
#include "C:/Program Files/MySQL/MySQL Server 8.0/include/mysqld_error.h"
#include <string>
#include <regex>
using namespace std;

void conexionDB();
bool cuiIsValid(const string &cui);
void verificadorLogin();
void apostar(MYSQL_ROW datos);
void cargarSaldo();

int q_estado;
MYSQL	*objDatos;
MYSQL_ROW filas;
MYSQL_RES* resultado;
int main() {
	
	conexionDB();
    
}

void conexionDB(){
	//intentar iniciar MySQL
	if(!(objDatos = mysql_init(0))) {
		cout << "ERROR: no conectada." << endl;
	};
	
	//conecto mi base de datos
	objDatos = mysql_real_connect(objDatos, "localhost", "root", "4dMindb&", "bdjuegotn2023", 3306, NULL, 0);
	
	if(objDatos){
		//verificadorLogin();
		cargarSaldo();
	}else{
		cout<<"ocurrio un error al intentar conectar con la base de datos";
	}
}

bool cuiIsValid(const string &cui) {
    if (cui.empty()) {
    	system("cls");
        cout<<"CUI vacio"<<endl;
        return true;
    }

	//validad la escritura del cui
    regex cuiRegExp(R"(^[0-9]{4}\s?[0-9]{5}\s?[0-9]{4}$)");

    if (!regex_match(cui, cuiRegExp)){
    	system("cls");
        cout<<"CUI con formato invalido"<<endl;
        return false;
    }

	//separamos por porciones el cui para validar sus respectivas partes
    string formattedCui = regex_replace(cui, regex("\\s"), "");
    int depto = stoi(formattedCui.substr(9, 2));
    int muni = stoi(formattedCui.substr(11, 2));
    string numero = formattedCui.substr(0, 8);
    int verificador = stoi(formattedCui.substr(8, 1));

	//listado de los departamentos con su cantidad de municipios
    int munisPorDepto[] = {
        /* 01 - Guatemala tiene:      */ 17 /* municipios. */,
        /* 02 - El Progreso tiene:    */  8 /* municipios. */,
        /* 03 - Sacatepéquez tiene:   */ 16 /* municipios. */,
        /* 04 - Chimaltenango tiene:  */ 16 /* municipios. */,
        /* 05 - Escuintla tiene:      */ 13 /* municipios. */,
        /* 06 - Santa Rosa tiene:     */ 14 /* municipios. */,
        /* 07 - Sololá tiene:         */ 19 /* municipios. */,
        /* 08 - Totonicapán tiene:    */  8 /* municipios. */,
        /* 09 - Quetzaltenango tiene: */ 24 /* municipios. */,
        /* 10 - Suchitepéquez tiene:  */ 21 /* municipios. */,
        /* 11 - Retalhuleu tiene:     */  9 /* municipios. */,
        /* 12 - San Marcos tiene:     */ 30 /* municipios. */,
        /* 13 - Huehuetenango tiene:  */ 32 /* municipios. */,
        /* 14 - Quiché tiene:         */ 21 /* municipios. */,
        /* 15 - Baja Verapaz tiene:   */  8 /* municipios. */,
        /* 16 - Alta Verapaz tiene:   */ 17 /* municipios. */,
        /* 17 - Petén tiene:          */ 14 /* municipios. */,
        /* 18 - Izabal tiene:         */  5 /* municipios. */,
        /* 19 - Zacapa tiene:         */ 11 /* municipios. */,
        /* 20 - Chiquimula tiene:     */ 11 /* municipios. */,
        /* 21 - Jalapa tiene:         */  7 /* municipios. */,
        /* 22 - Jutiapa				  */ 17 /* municipios. */
    };
	
	//si el numicipio es solo un dato nulo
    if (depto == 0 || muni == 0){
    	system("cls");
        cout<<"CUI con codigo de municipio o departamento invalido."<<endl;
        return false;
    }
	
	//di el codigo de departamento pasa la cantidad de 22 departamentos (penultimos dos numeros del dpi)
    if (depto > sizeof(munisPorDepto) / sizeof(munisPorDepto[0])){
    	system("cls");
        cout<<"CUI con codigo de departamento invalido."<<endl;
        return false;
    }
	
	//valida que el codigo de municipio sea el correcto (ultimos dos numeros del dpi)
    if (muni > munisPorDepto[depto - 1]){
    	system("cls");
        cout<<"CUI con codigo de municipio invalido."<<endl;
        return false;
    }

    // Se verifica el correlativo con base en el algoritmo del complemento 11.
    int total = 0;

    for(int i = 0; i < numero.length(); i++){
        total += (numero[i] - '0') * (i + 2);
    }

    int modulo = (total % 11);

    return modulo == verificador;
}

void verificadorLogin(){
	string cui, password;
	bool loginExitoso = false;
	
	while(!loginExitoso){
	    cout<<"Ingrese el CUI: ";
	    cin>>cui;
	    
	    if(cuiIsValid(cui)){
	        cout<<"Ingrese su contrasena: ";
	    	cin>>password;
	        string busqueda = "select * from cuenta where cui = " + cui + " and contrasena = '" + password + "';";
	        const char* B = busqueda.c_str();
	        q_estado = mysql_query(objDatos, B);
	    
	        if(!q_estado){
	        	resultado = mysql_store_result(objDatos);
	        	filas = mysql_fetch_row(resultado);
	        	if(filas != NULL){
	        		cout<<"bienvenido"<<endl;
					apostar(filas);
					loginExitoso = true;
				}else{
					system("cls");
					cout<<"Verifique su usuario y contrasena"<<endl;
				}
	        	
			}
	    } else {
	        cout<<"El CUI no es valido."<<endl;
	    }
	}

}

void apostar(MYSQL_ROW datos) {
	bool apuestaCorrecta = false;
	int saldoCuenta = atoi(datos[8]);//convierto el dato de esa parte de la tabla en un entero
	int saldoApostar;
	
	system("cls");
	
	while(!apuestaCorrecta){
		cout<<"Su saldo actual en la cunenta es de: Q"<<saldoCuenta<<endl;
		cout<<"Ingrese el saldo que desea apostar (multipo de 100 y menor al monto que tiene en su cuenta): ";
		cin>>saldoApostar;
		
		if((saldoApostar % 100) == 0 && saldoApostar < saldoCuenta){
			saldoCuenta -= saldoApostar; // resto la cantidad actual de la cuenta con el valor ingresado
		
			//actualizar el monto de la cuenta del usuario
			string apuesta = "update cuenta set saldo = "+ to_string(saldoCuenta) +" where idCuenta = "+ string(datos[0]) +";";
			const char* ap = apuesta.c_str();
		    q_estado = mysql_query(objDatos, ap);
		    
		    //mostar el saldo de la cuenta en pantalla (demo)
		    string saldo = "select saldo from cuenta where idCuenta = "+ string(datos[0]) +";";
		    const char* sa = saldo.c_str();
		    mysql_query(objDatos, sa);
		    resultado = mysql_store_result(objDatos);
		    filas = mysql_fetch_row(resultado);
			int saldoActual = atoi(filas[0]);
			cout<<"Ahora su Saldo actual de la cuenta es de: Q"<<saldoActual<<endl;
			apuestaCorrecta = true;
		}else{
			system("cls");
			cout<<"Por favor, ingrese una cantidad correcta"<<endl;
		}	
	}

}

void cargarSaldo(){
	string cui, password;
	bool login = false;
	
	while(!login){
		cout<<"Ingrese su numero de CUI: ";
		cin>>cui;
		
		if(cuiIsValid(cui)){
			cout<<"Ingrese su contrasena: ";
			cin>>password;
			string verificador = "select idCuenta, saldo from cuenta where cui = "+ cui +" and contrasena ='"+ password +"'";
			const char* veri = verificador.c_str();
			mysql_query(objDatos, veri);
			resultado = mysql_store_result(objDatos);
			filas = mysql_fetch_row(resultado);
			
			if(filas == NULL){
				system("cls");
				cout<<"Verifique su CUI o contrasena"<<endl;
				login = false;
			}
			
			if(filas != NULL){
				int saldoCargar = 0;
				int saldoActual = atoi(filas[1]);
				
				cout<<"Su saldo actual es de: Q"<<filas[1]<<endl;
				
				cout<<"Ingrese la candidad que desea cargar a su cuenta: ";
				cin>>saldoCargar;
				
				if((saldoCargar % 100) == 0 && saldoCargar > 0){
					saldoActual += saldoCargar;
				
					string saldoCargado = "update cuenta set saldo = "+ to_string(saldoActual) +" where idCuenta = "+ string(filas[0]) +";";
					const char* actualizar = saldoCargado.c_str();
					mysql_query(objDatos, actualizar);
					
					string dineroEnCuenta = "select saldo from cuenta where idCuenta = "+ string(filas[0]) +";";
					const char* dinero = dineroEnCuenta.c_str();
					mysql_query(objDatos, dinero);
					resultado = mysql_store_result(objDatos);
					filas = mysql_fetch_row(resultado);
					cout<<"Su saldo con lo que adiciono es de: Q"<<filas[0]<<endl;
					login = true;
				}else{
					system("cls");
					cout<<"Ingrese una cantidad mayor que 0 y multiplo de 100"<<endl;
				}
			}
		}else{
			cout<<"El CUI no es valido."<<endl;
		}
	}
	
	
}


