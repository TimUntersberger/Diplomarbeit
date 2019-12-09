import "dotenv/config"

module.exports = {
    client: "pg",
    connection: {
        host: process.env.APP_DB_HOST || "localhost",
        database: process.env.APP_DB_NAME || "db",
        user: process.env.APP_DB_USER || "user",
        password: process.env.APP_DB_PASS || "password",
    },
    migrations: {
        tableName: "knex_migrations"
    }
};
