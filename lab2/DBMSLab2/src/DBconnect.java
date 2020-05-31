import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

/**
 * @author Administrator
 *
 */
public class DBconnect {
    private String dbDriver="com.mysql.jdbc.Driver";
    private String dbUrl="jdbc:mysql://localhost/company?characterEncoding=utf8&useSSL=false";
    private String dbUser="root";
    private String dbPass="";

    public Connection getConn()
    {
        Connection conn=null;

        try {
            Class.forName(dbDriver);
        } catch (ClassNotFoundException e1) {
            // TODO 自动生成的 catch 块
            e1.printStackTrace();
        }

        try {
            conn = DriverManager.getConnection(dbUrl,dbUser,dbPass);
        } catch (SQLException e) {
            // TODO 自动生成的 catch 块
            e.printStackTrace();
        }
        return conn;
    }

    public void closeConn(Connection conn) {
        try {
            conn.close();
        } catch (SQLException e) {
            // TODO 自动生成的 catch 块
            e.printStackTrace();
        }
    }
}
