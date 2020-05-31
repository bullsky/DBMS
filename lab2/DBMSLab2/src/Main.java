import com.sun.org.apache.xerces.internal.impl.xpath.XPath;

import java.util.List;
import java.sql.Connection;
import java.util.LinkedList;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
//import DBconnect;
//import DbDao;
public class Main {

    public static void main(String[] args) {
        int len = args.length;
        if(len < 4) {
            System.out.println("usage:company_query –q <Number> -p [Parameters]");
            System.exit(0);
        }
        if(!args[0].equals("-q")| !args[2].equals("-p")){
            System.out.println("usage:company_query –q <Number> -p [Parameters]");
            System.exit(0);
        }

        int num = Integer.valueOf(args[1]);

        DBconnect connect;
        Connection conn;
        List<String> list = new LinkedList<String>();
        connect = new DBconnect();
        conn = connect.getConn();
        DbDao dao = new DbDao();
        switch (num) {
            case 1:
                if(len != 4) {
                    System.out.println("usage:company_query –q 1 -p %PNO%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String pno = args[3];
                String sql1 = "select EMPLOYEE.essn \n" +
                        "from EMPLOYEE,PROJECT,WORKS_ON \n" +
                        "where (EMPLOYEE.ESSN = WORKS_ON.ESSN) AND " +
                        "(PROJECT.PNO = WORKS_ON.PNO) AND " +
                        "(PROJECT.PNO = '"+pno+"');";
                list = dao.search(sql1,conn);
                for(int i=0;i<list.size();i++) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
            case 2:
                if(len != 4) {
                    System.out.println("usage:company_query –q 2 -p %PNAME%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String panme = args[3];
                String sql2 = "select ename \n" +
                        "from EMPLOYEE,PROJECT,WORKS_ON \n" +
                        "where (EMPLOYEE.ESSN = WORKS_ON.ESSN) AND " +
                        "(PROJECT.PNO = WORKS_ON.PNO) AND " +
                        "(PROJECT.PNAME = '"+panme+"');";
                list = dao.search(sql2,conn);
                for(int i=0;i<list.size();i++) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
            case 3:
                if(len != 4) {
                    System.out.println("usage:company_query –q 3 -p %DNAME%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String dname = args[3];
                String sql3 = "select ename,address\n" +
                        "from EMPLOYEE,DEPARTMENT \n" +
                        "where (EMPLOYEE.DNO = DEPARTMENT.DNO) AND " +
                        "(DEPARTMENT.DNAME= '"+dname+"');";
                list = dao.search(sql3,conn);
                for(int i=0;i<list.size();i+=2) {
                     System.out.println(list.get(i)+" "+list.get(i+1));
                }
                connect.closeConn(conn);
                break;
            case 4:
                if(len != 5) {
                    System.out.println("usage:company_query –q 4 -p %DNAME% %SALARY% ");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql4 = "select ename,address\n" +
                        "from EMPLOYEE,DEPARTMENT \n" +
                        "where (EMPLOYEE.DNO = DEPARTMENT.DNO) " +
                        "AND (EMPLOYEE.SALARY < "+args[4]+") " +
                        "AND (DEPARTMENT.DNAME= '"+args[3]+"');";
                list = dao.search(sql4,conn);
                for(int i=0;i<list.size();i+=2) {
                    System.out.println(list.get(i)+" "+list.get(i+1));
                }
                connect.closeConn(conn);
                break;
            case 5:
                if(len != 4) {
                    System.out.println("usage:company_query –q 5 -p %PNO%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql5 = "SELECT ENAME FROM EMPLOYEE\n" +
                        "where ename not in(\n" +
                        "select ename from EMPLOYEE,WORKS_ON\n" +
                        "where PNO = '"+ args[3] +
                        "' AND EMPLOYEE.ESSN=WORKS_ON.ESSN);";
                list = dao.search(sql5,conn);
                for(int i=0;i<list.size();i+=1) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
            case 6:
                if(len != 4) {
                    System.out.println("usage:company_query –q 6 -p %ENAME%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql6 = "SELECT E1.ENAME,DNAME\n" +
                        "FROM EMPLOYEE E1,EMPLOYEE E2,DEPARTMENT\n" +
                        "WHERE E2.ENAME= '" +args[3]+
                        "' AND E1.DNO = DEPARTMENT.DNO AND E1.SUPERSSN = E2.ESSN;";
                list = dao.search(sql6,conn);
                for(int i=0;i<list.size();i+=2) {
                    System.out.println(list.get(i)+" "+list.get(i+1));
                }
                connect.closeConn(conn);
                break;
            case 7:
                if(len != 5) {
                    System.out.println("usage:company_query –q 7 -p %PNO1% %PNO2% ");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql7 = "SELECT EMPLOYEE.ESSN\n" +
                        "FROM EMPLOYEE,WORKS_ON W1,WORKS_ON W2\n" +
                        "WHERE EMPLOYEE.ESSN = W1.ESSN AND EMPLOYEE.ESSN=W2.ESSN " +
                        "AND W1.PNO= '" + args[3] +
                        "' AND W2.PNO= '"+ args[4]+"' ;";
                list = dao.search(sql7,conn);
                for(int i=0;i<list.size();i+=1) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
            case 8:
                if(len != 4) {
                    System.out.println("usage:company_query –q 8 -p %SALARY%");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql8 = "SELECT DNAME\n" +
                        "FROM  EMPLOYEE NATURAL join DEPARTMENT\n" +
                        "GROUP BY DNO\n" +
                        "HAVING AVG(SALARY) < "+args[3]+";";
                list = dao.search(sql8,conn);
                for(int i=0;i<list.size();i+=1) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
            case 9:
                if(len != 5) {
                    System.out.println("usage:company_query –q 9 -p %N% %HOURS% ");
                    connect.closeConn(conn);
                    System.exit(0);
                }
                String sql9 = "SELECT ENAME\n" +
                        "FROM WORKS_ON NATURAL join EMPLOYEE\n" +
                        "GROUP BY ESSN\n" +
                        "HAVING COUNT(PNO)>= " + args[3] +
                        " AND SUM(HOURS)<= "+args[4]+" ;";
                list = dao.search(sql9,conn);
                for(int i=0;i<list.size();i+=1) {
                    System.out.println(list.get(i));
                }
                connect.closeConn(conn);
                break;
        }
        System.exit(0);
    }
}
