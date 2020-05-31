import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

/**
 * @author Administrator
 *
 */
public class DbDao {
    public int insert(String sql,Connection conn) {
        int flag = 0;
        PreparedStatement preStmt;
        try {
            preStmt = conn.prepareStatement(sql);
            flag=preStmt.executeUpdate();
        } catch (SQLException e) {
            // TODO 自动生成的 catch 块
            e.printStackTrace();
        }
        return flag;
    }

    public int counts(String tablename,Connection conn) {
        int flag = 0;
        String sql = "select count(*) as result from "+tablename;
        try
        {
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);
            while(rs.next())
            {
                flag = rs.getInt(1);
            }
        }
        catch (SQLException e)
        {
            e.printStackTrace();
        }
        return flag;
    }

    public List<String> search(String sql,Connection conn) {
        List<String> list = new LinkedList<String>();
        try
        {
            Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);
            while(rs.next())
            {
                for(int i = 1;i<=rs.getMetaData().getColumnCount();i++){
                    list.add(rs.getString(i));}
            }
        }
        catch (SQLException e)
        {
            e.printStackTrace();
        }
        return list;
    }
    public int edit(String sql,Connection conn) {
        int i = 0;
        PreparedStatement preStmt;
        try {
            preStmt = conn.prepareStatement(sql);
            i=preStmt.executeUpdate();
        } catch (SQLException e) {
            // TODO 自动生成的 catch 块
            e.printStackTrace();
        }
        return i;
    }
    public int delete(String sql,Connection conn) {
        int i = 0;
        try
        {
            Statement stmt = conn.createStatement();
            i = stmt.executeUpdate(sql);
        }
        catch (SQLException e)
        {
            e.printStackTrace();
        }

        return i;
    }
}
