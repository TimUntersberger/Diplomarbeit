import * as Knex from "knex";


export async function up(knex: Knex): Promise<any> {
    return knex.schema.createTable("firmware", (t) => {
        t.increments("id").primary();
        t.string("name");
        t.integer("updatedAt");
        t.integer("createdAt");
        t.integer("fileSize");
        t.string("fileName");
    })
}


export async function down(knex: Knex): Promise<any> {
    return knex.schema.dropTable("firmware");
}

