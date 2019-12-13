import knex from "knex";
import * as KnexConfig from "../../knexfile";

const conn = knex(KnexConfig);

export function queryToPromise<T>(query: knex.QueryBuilder, firstResultOnly?: boolean): Promise<T> {
    return new Promise((resolve, reject) => {
        query.then(firstResultOnly ? x => resolve(x[0]) : resolve).catch(reject)
    })
}

export default conn;